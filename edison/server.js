var Buffer = require('buffer').Buffer;
var fs = require('fs');
var controllerFile = 'controller';

var http = require('http');
var url = require('url');
var m = require('mraa'); //require mraa
var maxForwardSpeed = 4;

console.log('starting...');

var commandMotors = function(leftMotorSpeed, rightMotorSpeed) {
    console.log('writing')
    var myBuffer= new Buffer(3);
    myBuffer[0] = 255;
    myBuffer[1] = leftMotorSpeed;
    myBuffer[2] = rightMotorSpeed;
    console.log(fs.appendFileSync(controllerFile, myBuffer))
    console.log('finished writing');
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

commandMotors(48, 64);
commandMotors(100,101);
commandMotors(102,103);
http.createServer(function (req, res) {
    console.log(req)

    if (req.method == 'POST') {

        console.log(req.body);

        action = req.body.action;
        velocity = req.body.velocity;

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
        res.writeHead(200, {'Content-Type': 'text/plain'});
        req.end('You posted something.\n');

    } else {
        res.writeHead(200, {'Content-Type': 'text/plain'});
        res.end('Hello, world!\n');
    }

}).listen(1234);
