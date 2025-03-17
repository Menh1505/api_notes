#include "crow.h"
#include <iostream>

struct note
{
  int id;
  std::string title;
  std::string content;
  std::string timestamp;
};

std::vector<note> notes;

void createNote(crow::json::rvalue x)
{
  note n;
  n.id = notes.size();
  n.title = x["title"].s();
  n.content = x["content"].s();
  n.timestamp = x["timestamp"].s();
  notes.push_back(n);

  std::cout << "(" << __DATE__ << " " << __TIME__ << ")" << "[INFO] Added a new note with title: " << n.title << std::endl;
}

void putNote(int id, crow::json::rvalue x)
{
  notes[id].title = x["title"].s();
  notes[id].content = x["content"].s();
  notes[id].timestamp = x["timestamp"].s();
}

int main()
{
  crow::SimpleApp app;
  CROW_ROUTE(app, "/")([]()
                       { return "hello world!"; });

  CROW_ROUTE(app, "/notes").methods("GET"_method)([]
                                                  {
    crow::json::wvalue x;
    for (size_t i = 0; i < notes.size(); i++)
    {
        x[i]["id"] = notes[i].id;
        x[i]["title"] = notes[i].title;
        x[i]["content"] = notes[i].content;
        x[i]["timestamp"] = notes[i].timestamp;
    }
    return x; });

  CROW_ROUTE(app, "/notes/<int>").methods("GET"_method)([](int id)
                                                        {
    if(id < 0 || id >= notes.size())
    {
      return crow::response(404, "Note not found");
    }
    
    crow::json::wvalue x;
    note result = notes[id];

    x[result.title] = result.content;
    return crow::response(x.dump()); });

  CROW_ROUTE(app, "/notes").methods("POST"_method)([](const crow::request &req)
                                                   {
    auto x = crow::json::load(req.body);

    if (!x){
      return crow::response(400);
    }
                                                    
    createNote(x);
    return crow::response(201); });

  CROW_ROUTE(app, "/notes/<int>").methods("PUT"_method)([](const crow::request &req, int id)
                                                        {
    
    auto x = crow::json::load(req.body);
    if (!x || id < 0 || id >= notes.size())
    {
      return crow::response(400, "Invalid request or note ID");
    }
                                                    
    putNote(id, x);
    return crow::response(200); });

  CROW_ROUTE(app, "/notes/<int>").methods("DELETE"_method)([](int id)
                                                           {
    if(id < 0 || id >= notes.size())
    {
      return crow::response(404, "Note not found");
    }
                                                          
    notes.erase(notes.begin() + id);
    return crow::response(200); });

  app.port(3000).run();
}

// curl -X POST http://example.com/api -H "Content-Type: application/json" -d '{"key1":"value1", "key2":"value2"}'