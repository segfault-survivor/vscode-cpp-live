import * as fs from 'fs';
import * as path from 'path';

export function find_near(full_path: string, file_name: string): string | null
{
    let dir = path.dirname(full_path);

    while (true)
    {
        const target_path = path.join(dir, file_name);
        if (fs.existsSync(target_path))
        {
            return target_path;
        }

        const parent = path.dirname(dir);
        if (parent === dir)
        {
            break;
        }
        dir = parent;
    }

    return null;
}