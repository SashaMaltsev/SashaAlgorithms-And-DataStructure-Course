import os
import re
import argparse


description = "Удлинение файлов с указанным суффиксом до заданной длины путем присоединения к ним необходимого числа копий некоторого файла"

parser = argparse.ArgumentParser(description=description, add_help=False)

parser.add_argument("suffix", type=str)
parser.add_argument("file1", type=str)
parser.add_argument("size", type=int)
parser.add_argument("dir", nargs="?", type=str, default=str(os.getcwd()))
parser.add_argument("--help", "-h", action="help", help="Показать помощь")

args = parser.parse_args()
size_file1 = os.path.getsize(args.file1)

with open(args.file1) as file1:
    data = file1.read()

if size_file1 == 0:
    print("Размер копируемого файла не может быть 0")
    exit(1)

print("\tфайл\t\tразмер до\t\tразмер после")

count = 0
r = re.compile(r".*\." + args.suffix)

for file in filter(r.match, os.listdir(args.dir)):

    if file == args.file1:
        continue

    with open(args.dir + f"/{file}", "a") as file2:

        size_file2 = os.path.getsize(args.dir + f"/{file}")
        count += 1

        print(f"{count}\t{file}\t{size_file2}", end="\t\t\t")

        while size_file2 < args.size:
            file2.write(data)
            size_file2 += size_file1

        print(f"{size_file2}")
