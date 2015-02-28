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
    myBuffer[0] = 0xff;
    myBuffer[1] = leftMotorSpeed;
    myBuffer[2] = rightMotorSpeed;
    myBuffer[1] = 0x30;
    myBuffer[2] = 0x40;
    console.log(fs.appendFile(controllerFile, myBuffer, function(err) {if (err) console.log("ERROR!!" + err);}));
    console.log('finished writing');
}

var drive = function(forwardSpeed, rotationSpeed) {
    var leftMotorSpeed = forwardSpeed / maxForwardSpeed * 255;
    if (leftMotorSpeed == 255){
        leftMotorSpeed = 254;
    }
    var rightMotorSpeed = 255/2;
    // var  = 0.5*forwardSpeed + 0.5*rotationSpeed;
    // var rightMotorSpeed = 0.5*forwardSpeed - 0.5*rotationSpeed;
    commandMotors(leftMotorSpeed, rightMotorSpeed);
}

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
