#include "crow.h"

#include <string>
#include <vector>
#include <mutex>

struct Item {
    int id;
    std::string name;
};

int main()
{
    crow::SimpleApp app;

    std::vector<Item> items;
    int next_id = 1;
    std::mutex mtx;

    // Create
    CROW_ROUTE(app, "/items").methods("POST"_method)
    ([&](const crow::request& req){
        std::lock_guard<std::mutex> lock(mtx);
        auto x = crow::json::load(req.body);
        if (!x) {
            return crow::response(400);
        }
        Item item = {next_id++, x["name"].s()};
        items.push_back(item);
        crow::json::wvalue response_item;
        response_item["id"] = item.id;
        response_item["name"] = item.name;
        return crow::response(201, response_item);
    });

    // Read all
    CROW_ROUTE(app, "/items").methods("GET"_method)
    ([&](){
        std::lock_guard<std::mutex> lock(mtx);
        crow::json::wvalue response_items;
        for (size_t i = 0; i < items.size(); ++i) {
            response_items[i]["id"] = items[i].id;
            response_items[i]["name"] = items[i].name;
        }
        return crow::response(response_items);
    });

    // Read one
    CROW_ROUTE(app, "/items/<int>").methods("GET"_method)
    ([&](int id){
        std::lock_guard<std::mutex> lock(mtx);
        for (const auto& item : items) {
            if (item.id == id) {
                crow::json::wvalue response_item;
                response_item["id"] = item.id;
                response_item["name"] = item.name;
                return crow::response(response_item);
            }
        }
        return crow::response(404);
    });

    // Update
    CROW_ROUTE(app, "/items/<int>").methods("PUT"_method)
    ([&](const crow::request& req, int id){
        std::lock_guard<std::mutex> lock(mtx);
        auto x = crow::json::load(req.body);
        if (!x) {
            return crow::response(400);
        }
        for (auto& item : items) {
            if (item.id == id) {
                item.name = x["name"].s();
                crow::json::wvalue response_item;
                response_item["id"] = item.id;
                response_item["name"] = item.name;
                return crow::response(response_item);
            }
        }
        return crow::response(404);
    });

    // Delete
    CROW_ROUTE(app, "/items/<int>").methods("DELETE"_method)
    ([&](int id){
        std::lock_guard<std::mutex> lock(mtx);
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->id == id) {
                items.erase(it);
                return crow::response(204);
            }
        }
        return crow::response(404);
    });

    app.port(18080).multithreaded().run();

    return 0;
}
