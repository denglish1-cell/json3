#include "marshal.hpp"
#include "parse.hpp"
#include "json.hpp"
#include <sstream>

namespace
{
    void invalid(std::string const& message, json::Data const& where) {
        std::ostringstream out;
        out << message << " in " << where;
        throw MarshalError(out.str());
    }

    const json::Var& get_required_field(const std::map<std::string, json::Var>& m,
        const std::string& key,
        const json::Data& context) {
        auto it = m.find(key);
        if (it == m.end() || it->second->isNull()) {
            invalid("Missing required field: " + key, context);
        }
        return it->second;
    }

    // Helper: Dry out Task marshaling
    json::Var marshal_task(Task const& task) {
        json::Object tObj;
        auto& map = tObj.getObject();

        map["id"] = static_cast<double>(task.id);
        map["title"] = task.title;
        map["done"] = task.done;

        json::Array tags;
        for (const auto& tag : task.tags) tags.getArray().push_back(tag);
        map["tags"] = std::move(tags);

        map["completed"] = task.completed.has_value() ? json::Var(task.completed.value()) : json::Var(nullptr);

        return tObj;
    }
}

json::Var marshal(TodoList const& list)
{
    auto root = std::make_unique<json::Object>();
    auto& map = root->getObject();

    map["nextId"] = static_cast<double>(list.nextId);

    json::Array tasks;
    for (auto const& task : list.tasks) {
        tasks.getArray().push_back(marshal_task(task));
    }
    map["tasks"] = std::move(tasks);

    return json::Var(std::move(root));
}

TodoList unmarshal(json::Data& data) {
    if (!data.isObject()) invalid("Root JSON must be an object", data);

    const auto& rootMap = data.getObject();
    TodoList list;

    const auto& nextIdVar = get_required_field(rootMap, "nextId", data);
    if (!nextIdVar->isNumber()) invalid("nextId must be a number", data);
    list.nextId = static_cast<int>(nextIdVar->getNumber());

    const auto& tasksVar = get_required_field(rootMap, "tasks", data);
    if (!tasksVar->isArray()) invalid("tasks must be an array", data);

    for (const auto& taskVar : tasksVar->getArray()) {
        if (!taskVar->isObject()) invalid("Task entry must be an object", data);

        const auto& tMap = taskVar->getObject();
        Task t;

        t.id = static_cast<int>(get_required_field(tMap, "id", *taskVar)->getNumber());
        t.title = get_required_field(tMap, "title", *taskVar)->getString();
        t.done = get_required_field(tMap, "done", *taskVar)->getBoolean();

        const auto& tagsVar = get_required_field(tMap, "tags", *taskVar);
        if (!tagsVar->isArray()) invalid("tags must be an array", *taskVar);
        for (const auto& tagV : tagsVar->getArray()) t.tags.push_back(tagV->getString());

        auto it = tMap.find("completed");
        t.completed = (it != tMap.end() && !it->second->isNull()) ? std::optional(it->second->getString()) : std::nullopt;

        list.tasks.push_back(std::move(t));
    }

    return list;
}