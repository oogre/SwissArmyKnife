#!/usr/bin/env zx

const deviceNamesListFile = `device_names.txt`;
const deviceNamesListFileBackup = `${deviceNamesListFile}.backup`;

await $`cp ./${deviceNamesListFile} ./${deviceNamesListFileBackup}`;

let {stdout:deviceName} = await $`sed "${1}q;d" ${deviceNamesListFileBackup}; tail -n +2 ${deviceNamesListFileBackup} > ${deviceNamesListFileBackup}.tmp && mv ${deviceNamesListFileBackup}.tmp ${deviceNamesListFileBackup}`
deviceName = deviceName.replace("\n", "");

console.log(`DEVICE NAME IS : ${deviceName}`);

await $`cp ../src/main.cpp ../src/main.cpp.backup`;
let {stdout:maincpp} = await $`cat ../src/main.cpp`;
maincpp = maincpp.replace("__DEVICE_NAME__", deviceName);
await $`echo ${maincpp} > ../src/main.cpp`;

const p = $`platformio run --target upload --project-dir ../`;
for await (const chunk of p.stdout) {
  process.stdout.write(chunk)
}
await $`mv ../src/main.cpp.backup ../src/main.cpp`;
await $`mv ./${deviceNamesListFileBackup} ./${deviceNamesListFile}`;

console.log(`DONE! The device name is ${deviceName}`)
