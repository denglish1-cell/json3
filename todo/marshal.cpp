#include "marshal.hpp"
#include "parse.hpp"
#include "json.hpp"
#include <sstream>

namespace
{
    // Requirement: Throws MarshalError with the JSON context included in the message
    void invalid(std::string const& message, json::Data const& where) {
        std::ostringstream out;
        out << message << " in " << where;
        throw MarshalError(out.str());
    }

    json::Var marshal_task(Task const& task)
    {
        json::Object obj;
        auto& map = obj.getObject();

        map["id"] = static_cast<double>(task.id);
        map["title"] = task.title;
        map["done"] = task.done;

        json::Array tags;
        for (const auto& tag : task.tags) {
            tags.getArray().push_back(tag);
        }
        map["tags"] = std::move(tags);

        if (task.completed.has_value()) {
            map["completed"] = task.completed.value();
        } else {
            map["completed"] = nullptr;
        }

        return obj;
    }

    const json::Var& get_required_field(const std::map<std::string, json::Var>& m, const std::string& key, const json::Data& context) {
        auto it = m.find(key);
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
        taskArray.getArray().push_back(marshal_task(task));
    }
    map["tasks"] = std::move(taskArray);

    return root;
}

TodoList unmarshal(json::Data const& data) {
    if (!data.isObject()) {
        invalid("Root JSON must be an object", data);
    }

    const auto& rootMap = data.getObject();
    TodoList list;

    // Unmarshal nextId
    const auto& nextIdVar = get_required_field(rootMap, "nextId", data);
    if (!nextIdVar->isNumber()) invalid("nextId must be a number", data);
    list.nextId = static_cast<int>(nextIdVar->getNumber());

    // Unmarshal tasks array
    const auto& tasksVar = get_required_field(rootMap, "tasks", data);
    if (!tasksVar->isArray()) invalid("tasks must be an array", data);

    for (const auto& taskVar : tasksVar->getArray()) {
        if (!taskVar->isObject()) invalid("Task entry must be an object", data);
        const auto& tMap = taskVar->getObject();
        const json::Data& tData = *taskVar; // Context for errors

        Task t;

        t.id = static_cast<int>(get_required_field(tMap, "id", tData)->getNumber());
        t.title = get_required_field(tMap, "title", tData)->getString();
        t.done = get_required_field(tMap, "done", tData)->getBoolean();

        const auto& tagsVar = get_required_field(tMap, "tags", tData);
        if (!tagsVar->isArray()) invalid("tags must be an array", tData);
        for (const auto& tagV : tagsVar->getArray()) {
            t.tags.push_back(tagV->getString());
        }

        auto it = tMap.find("completed");
        if (it != tMap.end() && it->second->isString()) {
            t.completed = it->second->getString();
        } else {
            t.completed = std::nullopt;
        }

        list.tasks.push_back(std::move(t));
    }

    return list;
}