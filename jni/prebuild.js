const fs = require('fs');

let aura = fs.readFileSync('./aura.cpp').toString();
let output = fs.readFileSync('./com_eclipsesource_v8_V8Impl_src.cpp').toString();
output = output.replace('//AURA', aura);

fs.writeFileSync('./com_eclipsesource_v8_V8Impl.cpp', output);
