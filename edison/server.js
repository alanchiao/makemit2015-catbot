var Buffer = require('buffer').Buffer;
var fs = require('fs');
var controllerFile = '../controller';

var http = require('http');
var url = require('url');
var m = require('mraa'); //require mraa
var maxForwardSpeed = 4;

console.log('starting...');

var commandMotors = function(leftMotorSpeed, rightMotorSpeed) {
    var myBuffer= new Buffer(1);
    myBuffer[0] = 255;
    //myBuffer[1] = leftMotorSpeed;
    //myBuffer[2] = rightMotorSpeed;
    fs.appendFileSync(controllerFile, myBuffer);
}

var drive = function(leftMotorSpeed, rightMotorSpeed) {
    var leftMotorSpeed = parseInt(leftMotorSpeed / maxForwardSpeed * 255);
    if (leftMotorSpeed == 255){
        leftMotorSpeed = 254;
    }
    var rightMotorSpeed = parseInt(rightMotorSpeed / maxForwardSpeed * 255);
    if (rightMotorSpeed == 255) {
       rightMotorSpeed = 254;
    }

    commandMotors(leftMotorSpeed, rightMotorSpeed);
}

http.createServer(function (req, res) {
    //console.log(req)

    if (req.method == 'GET') {

        query = url.parse(req.url, true).query;
        action = query['action'];
        velocity = query['velocity'];
        console.log("action: " + action + ", velocity: " + velocity);
        if (action == 'MoveForward') {
            drive(velocity, velocity);
        }
        else if (action == 'TurnLeft') {
            drive(velocity, 0);
        }
        else if (action == 'TurnRight') {
            drive(0, velocity);
        }
        else if (action == 'Stop') {
            drive(0, 0);
        }
        else {
            console.log('Unknown action: ' + action)
        }

        // pipe the request data to the response to view on the web
        res.setHeader('Access-Control-Allow-Origin', '*');
        res.setHeader('Access-Control-Allow-Methods', 'GET');
        res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
        res.setHeader('Access-Control-Allow-Headers', 'Origin, Accept, X-Requested-With, Content-Type');
        res.setHeader('Access-Control-Allow-Credentials', true);
        res.writeHead(200, {'Content-Type': 'text/plain'});
        res.end('You posted something.\n');

    } else {
        res.writeHead(200, {'Content-Type': 'text/plain', 'Access-Control-Allow-Origin':'*'});
        res.end('Hello, world!\n');
    }

}).listen(8080);
