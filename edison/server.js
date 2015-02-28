var http = require('http');
var url = require('url');
var m = require('mraa'); //require mraa

http.createServer(function (req, res) {
    if (req.method == 'POST') {
        
        path = url.parse(req.url).path
        
        console.log(req.body)
        
        if (path == 'velocity') {
            console.log(req.body)
        }

        // pipe the request data to the response to view on the web
        res.writeHead(200, {'Content-Type': 'text/plain'});
        req.pipe(res);

    } else {
        res.writeHead(200, {'Content-Type': 'text/plain'});
        res.end('Hello, world!\n');
    }
    
}).listen(9615);
