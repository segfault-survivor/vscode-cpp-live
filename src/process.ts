import * as vscode from 'vscode';
import * as childProcess from 'child_process';
import treeKill from 'tree-kill';

function countOccurrences(str: string, char: string): number
{
    return (str.match(new RegExp(char, 'g')) || []).length;
}

function substringToNthOccurrence(source: string, char: string, n: number): [string, number]
{
    let index = -1;
    let occurrences = 0;

    // Loop through the source string to find the N-th occurrence of the character
    for (let i = 0; i < n; i++)
    {
        index = source.indexOf(char, index + 1);
        if (index === -1)
        {
            break;
        }
        ++occurrences;
    }

    // If the N-th occurrence is found, return the substring up to that point
    return [index === -1 ? source : source.substring(0, index + 1), occurrences];
}

export default class Process
{
    private output: vscode.OutputChannel;
    private max_lines: number;
    private cur_lines: number;
    private timestamp: boolean;
    private process: childProcess.ChildProcess | null = null;

    private pretty_print_data(data: any, prefix: string)
    {
        const d = data.toString();

        const lines_left = this.max_lines === 0 ? Infinity : this.max_lines - this.cur_lines;
        if (lines_left !== 0)
        {
            const [o, n] = substringToNthOccurrence(d, '\n', lines_left);
            this.cur_lines += n;

            if (this.timestamp)
            {
                const timestamp = new Date().toISOString().replace('T', ' ').replace('Z', '');
                const s = o.replace(/\r/g, "").replace(/(^$|\n)/g, `$1${timestamp} `);
                this.output.append(s);
            }
            else
            {
                this.output.append(o);
            }
        }
    }

    constructor(output: vscode.OutputChannel)
    {
        this.output = output;
        this.max_lines = 0;
        this.cur_lines = 0;
        this.timestamp = false;
        this.process = null;
    }

    public setMaxLines(value: number)
    {
        this.max_lines = value;
    }

    public setPrintTimestamp(value: boolean)
    {
        this.timestamp = value;
    }

    public is_running(): boolean
    {
        return this.process !== null && this.process.pid !== undefined && !this.process.killed && this.process.exitCode === null;
    }

    async wait_for_end()
    {
        return new Promise<number | NodeJS.Signals>
            (
                (resolve) =>
                {
                    if (this.is_running())
                    {
                        this.process!.once
                            (
                                'exit', (code: number | null, signal: NodeJS.Signals | null) =>
                            {
                                resolve(code === null ? signal! : code);
                            }
                            );
                    }
                    else
                    {
                        resolve("SIGLOST");
                    }
                }
            );
    }

    async stop()
    {
        if (this.process && this.process.pid)
        {
            await new Promise<void>
                (
                    (resolve, reject) =>
                    {
                        if (this.is_running())
                        {
                            treeKill
                                (
                                    this.process!.pid!, 'SIGKILL',
                                    (error) => 
                                    {
                                        error ? reject(error) : resolve();
                                    }
                                );
                        }
                        else
                        {
                            resolve();
                        }
                    }
                );
        }
    }

    public start(name: string, args: readonly string[], clear: boolean = true, cwd: string | undefined = undefined)
    {
        if (name.length === 0)
        {
            return this;
        }
        if (clear)
        {
            this.cur_lines = 0;
            this.output.clear();
        }
        this.pretty_print_data('', '');

        this.process = childProcess.spawn
            (
                name,
                args,
                {
                    windowsHide: true,
                    shell: true,
                    stdio: ['ignore', 'pipe', 'pipe'],
                    cwd: cwd
                }
            );

        if (this.process && this.process.stdout && this.process.stderr)
        {
            this.process.stdout.on
                (
                    'data', (data) => this.pretty_print_data(data, ' ')
                );

            this.process.stderr.on
                (
                    'data', (data) => this.pretty_print_data(data, '💀')
                );

            /*this.process.on
            (
              'exit', () =>
              {
                this.process = null;
              }
            );*/
        }
        return this;
    }

    public dispose()
    {
        this.stop();
    }
}
