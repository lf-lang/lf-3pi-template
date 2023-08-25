import * as fs from 'fs';
import * as path from 'path';
import { ConsoleLogger, LogLevel, RP2040 } from 'rp2040js';
import { bootromB1 } from './bootrom';
import { loadHex } from './intelhex';
//import { GDBTCPServer } from '../src/gdb/gdb-tcp-server';

const binDir = '../target/bin'
const curDir = '.'
const hexDir = `${curDir}/hex`
console.log("==== Running Tests ====");
// NOTE: emulator does not report segfaults, need to implement in src
async function readBinDir(path: string): Promise<string[]> {
    return new Promise<string[]>((resolve, reject) => {
        // TODO: maybe use async await 
        try {
            const dirFiles = fs.readdirSync(path, { withFileTypes: true });
            const hexFiles: string[] = dirFiles
                .filter((dirent) => dirent.isFile())
                .map((dirent) => dirent.name)
                .filter((fname) => fname.split('.').pop() === 'hex');
            console.log(hexFiles);
            resolve(hexFiles);
        } catch {
            reject("ERROR: invalid dir read");
        }
    });
}

async function runTestHex(path: string): Promise<string> {
    return new Promise<string>((resolve) => {
        const hex = fs.readFileSync(path, 'utf-8');
        const mcu = new RP2040();
        mcu.loadBootrom(bootromB1);
        mcu.logger = new ConsoleLogger(LogLevel.Error);
        loadHex(hex, mcu.flash, 0x10000000);
        
        // write uart out to file
        var uartOutFile = fs.createWriteStream(`${path}_out.txt`);
        mcu.uart[0].onByte = (value) => {
            uartOutFile.write(new Uint8Array([value]));
        };

        mcu.core.PC = 0x10000000; 
        mcu.execute();

        // MODIFY THE FOLLOWING
        // The following callback determines if a test has failed or passed
        // Currently a test only succeeds if it stops executing before 10 seconds

        setTimeout(() => {
            // TODO: check no error log
            if(!mcu.executing) {
                resolve('SUCCESS');
            } else {
                mcu.stop();
                // TODO: delete mcu
                resolve('FAIL');
            }
        }, 10000); // 10 seconds

    });
    //TODO: add gdb support
    //const gdbServer = new GDBTCPServer(mcu, 3333);
    //console.log(`RP2040 GDB Server ready! Listening on port ${gdbServer.port}`);
}

// type guards
const isRejected = (input: PromiseSettledResult<unknown>): input is PromiseRejectedResult => 
  input.status === 'rejected'

const isFulfilled = <T>(input: PromiseSettledResult<T>): input is PromiseFulfilledResult<T> => 
  input.status === 'fulfilled'

async function runAllTestHex(hexPaths: string[]): Promise<string> {
    const testRuns = hexPaths.map((path) => runTestHex(path));
    const results = await Promise.allSettled(testRuns);
    return new Promise<string>((resolve) => {
        console.log('Executed');
        const fulRes = results
            .filter(isFulfilled)
            .map((res, ind) => `${hexPaths[ind]}: ${res.value}`);
        console.log(fulRes);
        resolve('==== Tests Complete ====');
    });
}

readBinDir(curDir)
    .then((files) => {
        const paths = files.map((fname) => `${curDir}/${fname}`); 
        runAllTestHex(paths)
            .then((result) => {
                console.log(result);
            });
    })
    .catch((error) => console.log(error));

