var Buffer = require('buffer').Buffer;
var fs = require('fs');
var controllerFile = '../controller';

var http = require('http');
var url = require('url');
var m = require('mraa'); //require mraa
var maxForwardSpeed = 4;

console.log('starting...');

var commandMotors = function(leftMotorSpeed, rightMotorSpeed) {
    var myBuffer= new Buffer(3);
    myBuffer[0] = 255;
    myBuffer[1] = leftMotorSpeed;
    myBuffer[2] = rightMotorSpeed;
    fs.appendFileSync(controllerFile, myBuffer);
}

var drive = function(forwardSpeed, rotationSpeed) {
    var leftMotorSpeed = forwardSpeed / maxForwardSpeed * 255;
    if (leftMotorSpeed == 255){
        leftMotorSpeed = 254;
    }
    var rightMotorSpeed = 255/2; // var  = 0.5*forwardSpeed + 0.5*rotationSpeed;
    // var rightMotorSpeed = 0.5*forwardSpeed - 0.5*rotationSpeed;
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
            drive(velocity, 0);
        }
        else if (action == 'TurnLeft') {
            drive(0, -1*velocity);
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
