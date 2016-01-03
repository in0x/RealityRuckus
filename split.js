'use strict'

let fs = require('fs')
let readline = require('readline')
let stream = require('stream')

let instream = fs.createReadStream('html_names.txt')
let ostream = fs.createWriteStream('names.txt')
let outstream = new stream
let rl = readline.createInterface(instream, outstream)

rl.on('line', function(line) {
  
  let start = line.search('\">')
  let end = line.indexOf('<',start)
  
  let name = line.substr(start+2,end)
  name = name.split(' ')[0]
  name = name.split('<')[0]
  
  ostream.write(name + '\n')
  console.log(name + '\n')
})

rl.on('close', function() {
  // do something on finish here
})