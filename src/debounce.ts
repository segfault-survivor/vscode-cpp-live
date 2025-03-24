export class WaitValue
{
    public value: number;

    constructor(v: number)
    {
        this.value = v;
    }

    public setValue(v: number)
    {
        this.value = v;
    }

    public getValue()
    {
        return this.value;
    }
}

export function fun<T extends (...args: any[]) => any>(func: T, wait: WaitValue)
{
    let timeout: NodeJS.Timeout | null = null;

    return (...args: Parameters<T>): Promise<ReturnType<T>> => 
    {
        return new Promise
            (
                (resolve, reject) =>
                {
                    if (timeout !== null)
                    {
                        globalThis.clearTimeout(timeout);
                    }

                    timeout = globalThis.setTimeout
                        (
                            async () =>
                            {
                                try
                                {
                                    const result = await func(...args);
                                    resolve(result);
                                }
                                catch (error)
                                {
                                    reject(error);
                                }
                                finally
                                {
                                    timeout = null;
                                }
                            },
                            wait.getValue()
                        );
                }
            );
    };
}
