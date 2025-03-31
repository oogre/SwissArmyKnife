#!/usr/bin/env zx

const deviceNamesListFile = `device_names.txt`;

let {stdout:deviceName} = await $`sed "${1}q;d" ${deviceNamesListFile}; tail -n +2 ${deviceNamesListFile} > ${deviceNamesListFile}.tmp && mv ${deviceNamesListFile}.tmp ${deviceNamesListFile}`
deviceName = deviceName.replace("\n", "");

console.log(`DEVICE NAME IS : ${deviceName}`);

let {stdout:maincpp} = await $`cat ../src/main.cpp`;
await $`mv ../src/main.cpp ../src/main.cpp.backup`;
maincpp = maincpp.replace("__DEVICE_NAME__", deviceName);
await $`echo ${maincpp} > ../src/main.cpp`;
const p = $`platformio run --target upload --project-dir ../`;
for await (const chunk of p.stdout) {
  process.stdout.write(chunk)
}
await $`mv ../src/main.cpp.backup ../src/main.cpp`;

console.log(`DONE! The device name is ${deviceName}`)
