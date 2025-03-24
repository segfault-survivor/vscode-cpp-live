import Process from './process';
import * as debounce from "./debounce";
import * as filesystem from './filesystem';
import * as detect from './detect';
import * as config from './config';

import * as path from 'path';
import * as vscode from 'vscode';

const verbose = false;
function log(message?: any, ...optionalParams: any[]): void
{
    if (verbose)
    {
        console.log(message, ...optionalParams);
    }
}

class BatchFile
{
    public value: string;

    constructor()
    {
        this.value = "";
    }

    public setName(v: string)
    {
        this.value = v;
    }

    public getName()
    {
        return this.value;
    }
}

enum state_type
{
    stopped,
    running,
    stopping,
    saving
}

var the_state = state_type.stopped;
var the_wait_value = new debounce.WaitValue(config.getDebounce());

const onDidChangeTextDocument_debounce = debounce.fun
    (
        async (event: vscode.TextDocumentChangeEvent, process: Process, batch: BatchFile, context: vscode.ExtensionContext) =>
        {
            log(`onDidChangeTextDocument_debounce, event.document.isDirty = ${event.document.isDirty}`);
            if (event.document.isDirty)
            {
                log(`onDidChangeTextDocument_debounce: the_state = ${the_state}`);

                switch (the_state)
                {
                    case state_type.stopped:
                        // it has to be
                        break;
                    case state_type.running:
                        throw Error("In debounce while running");
                    case state_type.stopping:
                        throw Error("In debounce while stopping");
                    case state_type.saving:
                        log("onDidChangeTextDocument_debounce: returning");
                        return;
                }

                {
                    the_state = state_type.saving;
                    try
                    {
                        log("onDidChangeTextDocument_debounce: before saving");
                        await event.document.save();
                        log("onDidChangeTextDocument_debounce: after saving");
                    }
                    finally
                    {
                        if (the_state !== state_type.saving)
                        {
                            throw Error("state changed while saving");
                        }
                        the_state = state_type.stopped;
                        log("onDidChangeTextDocument_debounce: state = stopped");
                    }
                }
            }

            if (event.document.isDirty)
            {
                log("onDidChangeTextDocument_debounce: isDirty, returning");
                return;
            }
            else
            {
                log("onDidChangeTextDocument_debounce: before process.start");

                if (detect.isWindows() && config.getJobify())
                {
                    process.start
                        (
                            "PowerShell.exe",
                            [
                                "-ExecutionPolicy", "Bypass",
                                "-File", vscode.Uri.joinPath(context.extensionUri, `src`, `jobify.ps1`).fsPath,
                                batch.getName(),
                                event.document.fileName
                            ],
                            true,
                            path.dirname(batch.getName())
                        );
                }
                else
                {
                    process.start
                        (
                            batch.getName(),
                            [
                                event.document.fileName
                            ],
                            true,
                            path.dirname(batch.getName())
                        );
                }

                the_state = state_type.running;
                log("onDidChangeTextDocument_debounce: after process.start");
            }
        },
        the_wait_value
    );

async function onDidChangeTextDocument(event: vscode.TextDocumentChangeEvent, process: Process, batch: BatchFile, context: vscode.ExtensionContext)
{
    if
        (
        batch.getName() &&
        event.document.languageId === 'cpp' &&
        event.document.isDirty &&
        !event.document.isUntitled &&
        event.document.fileName &&
        config.getEnabled()
    )
    {
        log(`onDidChangeTextDocument: the_state = ${the_state}`);

        switch (the_state)
        {
            case state_type.stopped:
                // ok, not stopping, should debounce later
                break;
            case state_type.running:
                // going to stop it, then debounce
                {
                    log(`onDidChangeTextDocument: stopping`);
                    the_state = state_type.stopping;
                    const end = process.wait_for_end();
                    log(`onDidChangeTextDocument: got end`);
                    try
                    {
                        log(`onDidChangeTextDocument: before stop`);
                        await process.stop();
                        log(`onDidChangeTextDocument: after stop`);
                    }
                    catch (e)
                    {
                        log("exception thrown", e);
                    }
                    log(`onDidChangeTextDocument: before await end`);
                    await end;
                    log(`onDidChangeTextDocument: after await end`);
                    the_state = state_type.stopped;
                    log(`onDidChangeTextDocument: state = ${the_state}`);
                }
                break;
            case state_type.stopping:
                log(`onDidChangeTextDocument: returning, stopping in progress`);
                return;
            case state_type.saving:
                log(`onDidChangeTextDocument: returning, saving in progress`);
                return;
        }

        await onDidChangeTextDocument_debounce(event, process, batch, context);
    }
}

function is_date(): boolean
{
    const date = new Date();
    return date.getMonth() === 3 && date.getDate() === 1;
}

function UpdateStatusBarItem(bar: vscode.StatusBarItem, p: string)
{
    const enabled = config.getEnabled();
    bar.text = (p ? '$(pass)' : '$(error)') + ' C++Live ' + (enabled ? "$(play)" : (is_date() ? '$(snake)' : '$(debug-pause)'));
    bar.tooltip = 'Click = ' + (enabled ? 'disable' : 'enable') + '\n\n' + (p ? p : config.getProcessName() + ' not found');
}

function UpdateProcessName(editor: vscode.TextEditor | undefined, batch: BatchFile)
{
    function FindNearestByProcessName(editor: vscode.TextEditor | undefined)
    {
        if (editor && editor.document.uri.scheme === "file")
        {
            const p = filesystem.find_near(editor.document.fileName, config.getProcessName());
            return p ? p : "";
        }
        else
        {
            return "";
        }
    }

    batch.setName(FindNearestByProcessName(editor));
}

var open_example_in_progress = false;

function open_example(output: vscode.OutputChannel, context: vscode.ExtensionContext)
{
    if (open_example_in_progress)
    {

    }
    else
    {
        open_example_in_progress = true;

        detect.build(output, context.extensionUri).then
            (
                () => 
                {
                    open_example_in_progress = false;

                    const directory = vscode.Uri.joinPath(context.extensionUri, "example");

                    if (vscode.workspace.workspaceFolders && vscode.workspace.workspaceFolders.length > 0)
                    {
                        if (vscode.workspace.workspaceFolders[0].uri.fsPath === directory.fsPath)
                        {
                            vscode.window.showInformationMessage("C++Live example already opened");
                        }
                        else
                        {
                            vscode.commands.executeCommand("vscode.openFolder", directory, true);
                        }
                    }
                    else
                    {
                        vscode.commands.executeCommand('workbench.files.action.focusFilesExplorer');
                        vscode.workspace.updateWorkspaceFolders(0, null, { uri: directory });
                    }
                }
            );
    }
}

// repro for the bug https://github.com/microsoft/vscode/issues/82750
function bug_vscode_82750_step_1(output: vscode.OutputChannel)
{
    for (var t = 0; t !== 30; ++t)
    {
        for (var i = 1; i !== 10000; ++i)
        {
            output.append("hello".repeat(i) + '\n');
        }
        output.clear();
        output.append("one output\n");
    }
}

function bug_vscode_82750_step_2(output: vscode.OutputChannel)
{
    for (var i = 1; i !== 400; ++i)
    {
        output.append("one output " + i.toString() + "\n");
    }
}

function onDidChangeConfiguration(statusBarItem: vscode.StatusBarItem, process: Process, batch: BatchFile)
{
    UpdateProcessName(vscode.window.activeTextEditor, batch);
    UpdateStatusBarItem(statusBarItem, batch.getName());

    process.setMaxLines(config.getMaxLines());
    process.setPrintTimestamp(config.getPrintTimestamp());

    the_wait_value.setValue(config.getDebounce());
}

function onDidChangeActiveTextEditor(editor: vscode.TextEditor | undefined, process: BatchFile, statusBarItem: vscode.StatusBarItem)
{
    UpdateProcessName(editor, process);
    UpdateStatusBarItem(statusBarItem, process.getName());
}

export function activate(context: vscode.ExtensionContext)
{
    const output = vscode.window.createOutputChannel('C++Live');
    output.show();

    detect.example_detect(output, context);

    const process = new Process(output);
    var batch = new BatchFile();

    const statusBarItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left, 0);
    statusBarItem.command = 'cpp-live.Toggle';

    onDidChangeActiveTextEditor(vscode.window.activeTextEditor, batch, statusBarItem);

    statusBarItem.show();

    context.subscriptions.push
        (
            statusBarItem,
            vscode.commands.registerCommand('cpp-live.Toggle', () => config.toggleEnable()),
            vscode.commands.registerCommand('cpp-live.open_example', () => open_example(output, context)),
            vscode.commands.registerCommand('cpp-live.bug_vscode_82750_step_1', () => bug_vscode_82750_step_1(output)),
            vscode.commands.registerCommand('cpp-live.bug_vscode_82750_step_2', () => bug_vscode_82750_step_2(output)),
            vscode.workspace.onDidChangeConfiguration(() => onDidChangeConfiguration(statusBarItem, process, batch)),
            vscode.workspace.onDidChangeTextDocument(async (event) => onDidChangeTextDocument(event, process, batch, context)),
            vscode.window.onDidChangeActiveTextEditor((e: vscode.TextEditor | undefined) => onDidChangeActiveTextEditor(e, batch, statusBarItem)),
            { dispose: () => process.dispose() }
        );
}

export function deactivate() { }
