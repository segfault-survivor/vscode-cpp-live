import Process from './process';
import * as vscode from 'vscode';

export function example_detect(output: vscode.OutputChannel, context: vscode.ExtensionContext)
{
    const uri = context.extensionUri;

    set_example_one(output, uri, "git");
    set_example_one(output, uri, "llvm");
    if (isWindows())
    {
        set_example_one(output, uri, "msvc");
    }
}

export function isWindows()
{
    return process.platform === 'win32';
}

export function getProcessName()
{
    return "c++live." + (isWindows() ? 'bat' : 'sh');
}

function set_context(name: string, value: boolean)
{
    vscode.commands.executeCommand('setContext', 'cpp-live.' + name, value);
}

function get_example_executable_name(uri: vscode.Uri)
{
    return vscode.Uri.joinPath(uri, `example/${getProcessName()}`).fsPath;
}

function set_example_one(output: vscode.OutputChannel, uri: vscode.Uri, name: string)
{
    new Process(output)
        .start(get_example_executable_name(uri), ["detect_" + name], false)
        .wait_for_end()
        .then
        (
            (a) => set_context(`check-${name}`, a === 0)
        );
}

export function build(output: vscode.OutputChannel, uri: vscode.Uri)
{
    return new Process(output)
        .start(get_example_executable_name(uri), ["build_only"])
        .wait_for_end();
}