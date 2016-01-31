var http = require('http');
var url = require('url');
var data = {}

function onRequest(request, response){
   console.log("A user made a request: "+request.url);
   var parts = url.parse(request.url, true);

   if(parts.query['id']){
      var id = parts.query['id']
      if(parts.query['data']){
         data[id] = JSON.parse(parts.query['data']);
      }
      var out = "No output.";
      if(data[id]){
         console.log("ID good");
         out = JSON.stringify(data[id]);
      }
      response.writeHead('200', {"Content-Type": "text/plain"});
      response.write(out);
      response.end();
      if(parts.query['over']){
         data[id] = JSON.parse(parts.query['over']);
      }
   }else{
      console.log("ID bad");
      response.writeHead('200', {"Content-Type": "text/plain"});
      response.write("Please provide a valid ID.");
      response.end();
   }
}

http.createServer(onRequest).listen(80);
console.log("Server is now running.");
