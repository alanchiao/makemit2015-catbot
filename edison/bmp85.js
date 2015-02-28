var m = require('mraa'); //require mraa

// helper function to go from hex val to dec
function char(x) { return parseInt(x, 16); }

x = new m.I2c(0)
x.address(0x77)

// initialise device
if (x.readReg(char('0xd0')) != char('0x55')) {
  console.log("error");
}

// we want to read temperature so write 0x2e into control reg
x.writeReg(char('0xf4'), char('0x2e'))

// read a 16bit reg, obviously it's uncalibrated so mostly a useless value :)
console.log(x.readWordReg(char('0xf6')))

// and we can do the same thing with the read()/write() calls if we wished
// thought I'd really not recommend it!
buf = new Buffer(2)
buf[0] = char('0xf4')
buf[1] = char('0x2e')
console.log(buf.toString('hex'))
x.write(buf)

x.writeByte(char('0xf6'))
d = x.read(2)
