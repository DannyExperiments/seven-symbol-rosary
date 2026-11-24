#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
using namespace std;
using Mask = uint32_t;
using Word = string;
static long long parents[8], solutions[8], unique_nodes[8];
static unordered_set<Word> seen[8];
static int limit, target;
static bool found;
static Word first_witness;

// Lexicographically least restricted-growth string over rotations and reflections.
Word canonical(const Word& w) {
    Word best(w.size(),char(127));
    for (int dir : {-1,1}) for (int i=0;i<int(w.size());++i) {
        int map[8]={}, next=0, p=i;
        Word v;
        for (int j=0;j<int(w.size());++j) {
            int a=w[p];
            if (!map[a]) map[a]=++next;
            v.push_back(char(map[a]));
            if(v[j]>best[j] && v.compare(0,j,best,0,j)==0) break;
            p=(p+dir+int(w.size()))%int(w.size());
        }
        if(v.size()==w.size() && v<best) best=v;
    }
    return best;
}

// For each permutation of the parent, find gaps (immediately before parent
// position i) from which one traversal spells it. Keep inclusion-minimal masks.
vector<Mask> obligations(const Word& w,int k) {
    int n=w.size();
    vector<array<int,8>> nxt(2*n+1);
    nxt[2*n].fill(2*n);
    for(int j=2*n-1;j>=0;--j){
        nxt[j]=nxt[j+1];
        nxt[j][int(w[j%n])]=j;
    }
    vector<int> perm;
    for(int a=1;a<=k;++a) perm.push_back(a);
    vector<Mask> masks;
    do {
        Mask cuts=0;
        for(int start=0;start<n;++start) {
            int p=start;
            for(int a:perm) {
                p=nxt[p][a]+1;
                if(p>start+n) break;
            }
            if(p<=start+n) cuts|=Mask(1)<<start;
        }
        masks.push_back(cuts);
    }while(next_permutation(perm.begin(),perm.end()));
    sort(masks.begin(), masks.end(),[](Mask a,Mask b){
        int aa=__builtin_popcount(a),bb=__builtin_popcount(b);
        return aa!=bb ? aa<bb : a<b;
    });
    vector<Mask> minimal;
    for(Mask m:masks){
        bool implied=false;
        for(Mask q:minimal) if((q&m)==q){implied=true;break;}
        if(!implied) minimal.push_back(m);
    }
    return minimal;
}

void expand(const Word& w,int k,int largest) {
    if(found) return;
    ++parents[k];
    int n=w.size();
    int maxadd=(limit-n)/(target-k);
    if(maxadd<largest) return;
    auto need=obligations(w,k);
    vector<int> gaps(n);
    auto search=[&](auto&& self,int start,int remaining,Mask used)->void {
        if(found) return;
        if(remaining==0){
            for(Mask m:need) if(!(used&m)) return;
            ++solutions[k+1];
            Word child;
            int added=0;
            for(int i=0;i<n;++i){
                child.append(gaps[i],char(k+1));
                added+=gaps[i];
                child.push_back(w[i]);
            }
            if(k+1==target){found=true;first_witness=child;return;}
            if(seen[k+1].insert(canonical(child)).second) {
                ++unique_nodes[k+1];
                expand(child,k+1,added);
            }
            return;
        }
        Mask available=~((Mask(1)<<start)-1);
        Mask unavailable=0;
        int lower=0;
        for(Mask m:need) if(!(used&m)) {
            Mask choices=m&available;
            if(!choices) return;
            if(!(choices&unavailable)){
                unavailable|=choices; ++lower;
                if(lower>remaining) return;
            }
        }
        for(int i=start;i<n;++i){
            ++gaps[i];
            self(self,i,remaining-1,used|(Mask(1)<<i));
            --gaps[i];
        }
    };
    int minadd=k+1==target?limit-n:largest;
    for(int a=minadd;a<=maxadd;++a) search(search,0,a,0);
}

int main(int argc,char** argv){
    if(argc!=3) return 2;
    target=atoi(argv[1]); limit=atoi(argv[2]);
    if(target<2||target>7||limit<target||limit>30) return 2;
    for(int s=1;s<=limit/target && !found;++s) expand(Word(s,char(1)),1,s);
    cout<<"n="<<target<<" L="<<limit<<" exists="<<found<<" witness=";
    for(char c:first_witness) cout<<int(c);
    cout<<"\nlevel parents accepted_insertions distinct_children\n";
    for(int k=1;k<=target;++k) cout<<k<<' '<<parents[k]<<' '<<solutions[k]<<' '<<unique_nodes[k]<<'\n';
}
