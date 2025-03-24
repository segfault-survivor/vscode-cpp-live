import * as detect from './detect';
import * as vscode from 'vscode';

function getConfig()
{
    return vscode.workspace.getConfiguration('cpp-live');
}

export function getEnabled()
{
    return getConfig().get<boolean>('enable', true);
}

export function getMaxLines()
{
    return Math.abs(getConfig().get<number>('maxLines', 1000));
}

export function getPrintTimestamp()
{
    return getConfig().get<boolean>('printTimestamp', false);
}

export function getDebounce()
{
    return Math.abs(getConfig().get<number>('debounce', 300));
}

export function getJobify()
{
    return getConfig().get<boolean>('jobify', true);
}

export function toggleEnable()
{
    getConfig().update('enable', !getEnabled(), vscode.ConfigurationTarget.Global);
}

export function getProcessName()
{
    return detect.getProcessName();
}
