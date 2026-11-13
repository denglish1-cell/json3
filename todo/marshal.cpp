#include "marshal.hpp"
#include "parse.hpp"
#include "json.hpp"

#include <sstream>
#include <string>

namespace
{
    // Helper: throw a MarshallError exception
    void invalid(std::string const& message, json::Data const& where) {
        std::ostringstream out;
        out << message << " in " << where;
        throw MarshalError(out.str());
    }

    json::Var marshal_task(Task const& task)
    {
        json::Array tags;
        for (auto const& tag : task.tags) {
            tags.getArray().emplace_back(tag);
        }

        json::Object obj;
        auto& map = obj.getObject();
        map["id"]    = static_cast<double>(task.id);
        map["title"] = task.title;
        map["done"]  = task.done;
        map["tags"]  = std::move(tags);

        if (task.completed.has_value()) {
            map["completed"] = *task.completed;
        } else {
            map["completed"] = nullptr;
        }

        return obj;
    }

    const json::Var& get_field(const std::map<std::string, json::Var>& map, const std::string& key) {
        const auto it = map.find(key);
        if (it == map.end() || it->second->isNull()) {
            throw MarshalError("Missing required field: " + key);
        }
        return it->second;
    }
}


json::Var marshal(TodoList const& list)
{
    json::Array tasks;
    for (auto const& task : list.tasks) {
        tasks.getArray().push_back(marshal_task(task));
    }

    json::Object root;
    auto& map = root.getObject();
    map["nextId"] = static_cast<double>(list.nextId);
    map["tasks"]  = std::move(tasks);

    return root;
}

TodoList unmarshal(json::Data& data) {
    if (!data.isObject()) {
        throw MarshalError("Root JSON must be an object");
    }

    std::map<std::string, json::Var> rootMap = data.getObject();
    TodoList list;

    // Unmarshal nextId
    const auto& nextIdVar = get_field(rootMap, "nextId");
    if (!nextIdVar->isNumber()) throw MarshalError("nextId must be a number");
    list.nextId = static_cast<int>(nextIdVar->getNumber());

    // Unmarshal tasks array
    const auto& tasksVar = get_field(rootMap, "tasks");
    if (!tasksVar->isArray()) throw MarshalError("tasks must be an array");

    for (const auto& taskVar : tasksVar->getArray()) {
        if (!taskVar->isObject()) throw MarshalError("Each task must be an object");
        const auto& tMap = taskVar->getObject();

        Task t;

        // id
        const auto& idV = get_field(tMap, "id");
        if (!idV->isNumber()) throw MarshalError("task id must be a number");
        t.id = static_cast<int>(idV->getNumber());

        // title
        const auto& titleV = get_field(tMap, "title");
        if (!titleV->isString()) throw MarshalError("task title must be a string");
        t.title = titleV->getString();

        // done
        const auto& doneV = get_field(tMap, "done");
        if (!doneV->isBoolean()) throw MarshalError("task done must be a boolean");
        t.done = doneV->getBoolean();

        // tags
        const auto& tagsV = get_field(tMap, "tags");
        if (!tagsV->isArray()) throw MarshalError("tags must be an array");
        for (const auto& tag : tagsV->getArray()) {
            if (!tag->isString()) throw MarshalError("tag must be a string");
            t.tags.push_back(tag->getString());
        }

       // String or NULL
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

