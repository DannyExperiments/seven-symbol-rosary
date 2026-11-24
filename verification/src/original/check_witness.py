"""Greedy scanning of each complete rotation for every labeled permutation."""
from itertools import permutations

def check(word, alphabet):
    rotations = [word[start:] + word[:start] for start in range(len(word))]
    hits = 0
    misses = 0
    for order in permutations(alphabet):
        for rotation in rotations:
            available = iter(rotation)
            if all(letter in available for letter in order):
                hits += 1
                break
        else:
            misses += 1
    print(f"{word}  length={len(word)} coverage={hits}/{hits+misses}")
    return misses

from pathlib import Path
solution = Path(__file__).with_name('word.txt').read_text().strip()
assert len(solution)==24 and set(solution)==set('1234567')
assert check(solution,'1234567')==0
assert check("01523452104352543","012345")==0
assert check("1234567","1234567")>0
