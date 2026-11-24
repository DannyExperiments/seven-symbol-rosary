"""Cross-check the optimized profile solver against direct labeled permutation tests."""
import itertools
import os
import subprocess
from collections import Counter

def profile_witness(profile):
    key=','.join(map(str,profile))
    out=subprocess.check_output([os.environ['PROFILE_SOLVER'],key],text=True)
    line=next(line for line in out.splitlines() if line.startswith('profile '))
    word=line.split(' witness ')[1].split()[0]
    return None if word=='NONE' else tuple(map(int,word))

def is_universal(word,n):
    for permutation in itertools.permutations(range(n)):
        for start in range(len(word)):
            rotation=iter(word[start:]+word[:start])
            if all(symbol in rotation for symbol in permutation):
                break
        else:
            return False
    return True

def restricted_words(n,length):
    """Every linear word up to renaming has a unique first-appearance labeling."""
    def generate(word,used):
        if len(word)==length:
            if used==n: yield word
            return
        for letter in range(min(n,used+1)):
            yield from generate(word+(letter,),max(used,letter+1))
    yield from generate((),0)

tested=0
for n,maxlength in [(2,7),(3,8),(4,9)]:
    for length in range(n,maxlength+1):
        profiles={}
        for word in restricted_words(n,length):
            profile=tuple(sorted(Counter(word).values()))
            if profile not in profiles or not profiles[profile]:
                profiles[profile]=is_universal(word,n)
        for profile,exists in profiles.items():
            witness=profile_witness(profile)
            assert exists==(witness is not None),(n,profile,exists,witness)
            if witness:
                assert tuple(sorted(Counter(witness).values()))==profile
                assert is_universal(witness,n)
            tested+=1
    print('direct comparison passed',n,'symbols through length',maxlength)
print('number of profile tests',tested)
