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
        // The prompt says: "It's OK to look up the value and report it missing if its value is null."
        if (it == m.end() || it->second->isNull()) {
            invalid("Missing required field: " + key, context);
        }
        return it->second;
    }
}

json::Var marshal(TodoList const& list)
{
    json::Object root;
    auto& map = root.getObject();

    map["nextId"] = static_cast<double>(list.nextId);

    json::Array taskArray;
    for (auto const& task : list.tasks) {
        json::Object tObj;
        auto& tMap = tObj.getObject();

        tMap["id"] = static_cast<double>(task.id);
        tMap["title"] = task.title;
        tMap["done"] = task.done;

        json::Array tags;
        for (const auto& tag : task.tags) {
            tags.getArray().push_back(tag);
        }
        tMap["tags"] = std::move(tags);

        if (task.completed.has_value()) {
            tMap["completed"] = task.completed.value();
        }
        else {
            tMap["completed"] = nullptr;
        }

        taskArray.getArray().push_back(std::move(tObj));
    }
    map["tasks"] = std::move(taskArray);

    return root;
}

TodoList unmarshal(json::Data& data) {
    if (!data.isObject()) {
        invalid("Root JSON must be an object", data);
    }

    const auto& rootMap = data.getObject();
    TodoList list;

    // Use get_required_field for nextId
    const auto& nextIdVar = get_required_field(rootMap, "nextId", data);
    if (!nextIdVar->isNumber()) invalid("nextId must be a number", data);
    list.nextId = static_cast<int>(nextIdVar->getNumber());

    const auto& tasksVar = get_required_field(rootMap, "tasks", data);
    if (!tasksVar->isArray()) invalid("tasks must be an array", data);

    for (const auto& taskVar : tasksVar->getArray()) {
        // Ensure the task element is an object
        if (!taskVar->isObject()) invalid("Task entry must be an object", data);

        const auto& tMap = taskVar->getObject();

        Task t;
        t.id = static_cast<int>(get_required_field(tMap, "id", *taskVar)->getNumber());
        t.title = get_required_field(tMap, "title", *taskVar)->getString();
        t.done = get_required_field(tMap, "done", *taskVar)->getBoolean();

        const auto& tagsVar = get_required_field(tMap, "tags", *taskVar);
        if (!tagsVar->isArray()) invalid("tags must be an array", *taskVar);

        for (const auto& tagV : tagsVar->getArray()) {
            t.tags.push_back(tagV->getString());
        }

        auto it = tMap.find("completed");
        if (it != tMap.end() && !it->second->isNull()) {
            if (!it->second->isString()) invalid("completed must be a string", *taskVar);
            t.completed = it->second->getString();
        }
        else {
            t.completed = std::nullopt;
        }

        list.tasks.push_back(std::move(t));
    }

    return list;
}