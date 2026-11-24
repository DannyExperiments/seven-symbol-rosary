"""Test every sorted multiplicity partition unless a witness proves existence."""
import os
import subprocess
import sys


def partitions(total, parts, least=1):
    if parts == 1:
        if total >= least:
            yield [total]
        return
    for first in range(least, total // parts + 1):
        for rest in partitions(total - first, parts - 1, first):
            yield [first, *rest]


alphabet_size, length = map(int, sys.argv[1:3])
expected = sys.argv[3] if len(sys.argv) > 3 else None
profile_count = 0
core_count = 0
found = False
for profile in partitions(length, alphabet_size):
    argument = ','.join(map(str, profile))
    output = subprocess.check_output([os.environ['PROFILE_SOLVER'], argument], text=True)
    record = next(line for line in output.splitlines() if line.startswith('profile '))
    print(record, flush=True)
    profile_count += 1
    core_count += int(record.split()[-1])
    if 'witness NONE' not in record:
        print('EXISTS', alphabet_size, length, argument, flush=True)
        found = True
        break
if not found:
    print('EXHAUSTED', alphabet_size, length, 'profiles', profile_count,
          'final cores', core_count, flush=True)
if expected and expected != ('exists' if found else 'absent'):
    sys.exit('Search result contradicts the proposed bound.')
