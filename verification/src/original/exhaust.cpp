#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>
using namespace std;
using Mask=uint32_t;
int counts[8],n;
long long nodes[8]={}, raw[8]={},calls=0;
vector<array<int,7>> orders[8];
string witness;
int popc(Mask x){return __builtin_popcount(x);}
int lowbit(Mask x){return __builtin_ctz(x);}
// Canonical under rotation, reflection and arbitrary equal-multiplicity renaming.
// The initial letter gets the smallest label exactly when its frequency is minimal.
string canonical(const string&w,int k){
    int L=w.size(), first[8]={};
    for(int x=0;x<k;x++)first[x]= x && counts[x-1]==counts[x] ? first[x-1] : x;
    string best(L,'9'),cur(L,'0');
    for(int j=0;j<L;j++)if(first[w[j]-'0']==0)for(int dir:{-1,1}){
        int next[8];copy(first,first+8,next);int assigned[8];fill(assigned,assigned+8,-1);
        int cmp=0,i=j,t=0;
        for(;t<L;t++){
            int x=w[i]-'0',g=first[x];
            if(assigned[x]<0)assigned[x]=next[g]++;
            cur[t]='0'+assigned[x];
            if(cmp==0){if(cur[t]>best[t])break;cmp=(cur[t]<best[t])?-1:0;}
            i+=dir;if(i==L)i=0;if(i<0)i=L-1;
        }
        if(t==L&&cmp<0)best=cur;
    }
    return best;
}
struct Placement {
    const string &w;
    int k,L,m;
    vector<Mask> edges;
    Mask all;
    unordered_set<string> seen;
    array<int,32> extra{};
    Placement(const string &s,int newlabel):w(s),k(newlabel),L(w.size()),m(counts[k]),all((1u<<L)-1){}
    bool build(){
        array<array<int,7>,50> next;
        next[2*L].fill(2*L);
        for(int i=2*L-1;i>=0;i--) {
            next[i]=next[i+1];next[i][w[i%L]-'0']=i;
        }
        for(const auto&p:orders[k]){
            Mask good=0;
            for(int start=0;start<L;){
                int first=next[start][p[0]],last=first;
                for(int j=1;j<k&&last<2*L;j++)last=next[last+1][p[j]];
                int end=min(first,L-1),begin=max(start,last-L+1);
                if(begin<=end)good|=(1u<<(end+1))-(1u<<begin);
                start=end+1;
            }
            if(!good)return false;
            if(good!=all)edges.push_back(good);
        }
        sort(edges.begin(),edges.end(),[](Mask a,Mask b){int sa=popc(a),sb=popc(b);return sa!=sb?sa<sb:a<b;});
        edges.erase(unique(edges.begin(),edges.end()),edges.end());
        vector<Mask> minimal;
        for(Mask e:edges){
            bool redundant=false;
            for(Mask f:minimal)if((e&f)==f){redundant=true;break;}
            if(!redundant)minimal.push_back(e);
        }
        edges.swap(minimal);
        return true;
    }
    bool visit(Mask chosen) {
        string c;for(int i=0;i<L;i++){c.append(extra[i]+((chosen>>i)&1),'0'+k);c+=w[i];}
        if(k+1==n){witness=c;return true;}
        raw[k+1]++;
        string key=canonical(c,k+1);
        if(!seen.insert(key).second)return false;
        if(k>0&&counts[k-1]==counts[k]){
            string parent;for(char ch:key)if(ch!='0'+k)parent+=ch;
            if(canonical(parent,k)!=w)return false;
        }
        nodes[k+1]++;
        Placement next(key,k+1);
        return next.run();
    }
    bool fill_extra(Mask chosen,Mask options,int left) {
        if(!left)return visit(chosen);
        while(options){
            int idx=lowbit(options);
            extra[idx]++;
            if(fill_extra(chosen,options,left-1))return true;
            extra[idx]--;
            options&=options-1;
        }
        return false;
    }
    bool covers(Mask chosen,Mask allowed,int left) {
        Mask split=0;int width=100;
        // Disjoint unsatisfied constraints each require a separate selected gap.
        Mask occupied=0;int needed=0;
        for(Mask e:edges)if(!(e&chosen)){
            Mask avail=e&allowed;
            if(!avail||left==0)return false;
            if(!(avail&occupied)){occupied|=avail;if(++needed>left)return false;}
            int v=popc(avail);if(v<width){split=avail;width=v;}
        }
        if(!split){
            if(k+1==n && left){ // One valid support suffices. Pad to the specified multiplicity in any selected gap.
                int idx=chosen?lowbit(chosen):0;extra[idx]+=left;
                bool ok=visit(chosen);
                extra[idx]-=left;return ok;
            }
            return fill_extra(chosen,chosen|allowed,left);
        }
        while(split) {
            Mask bit=split&-split;split-=bit;allowed&=~bit;
            if(covers(chosen|bit,allowed,left-1))return true;
        }
        return false;
    }
    bool run(){
        if(!build())return false;
        if(k+1==n && ++calls%1000000==0)cout<<"#final "<<calls<<" #parents "<<nodes[k]<<endl;
        return covers(0,all,m);
    }
};
int main(int argc,char**argv){
    if(argc!=2)return 2;
    string tuple=argv[1];
    if(tuple.find(',')!=string::npos) {
        stringstream parts(tuple); string token;
        while(getline(parts,token,',')) {
            if(n>=7 || token.empty() || token.size()>2 || token.find_first_not_of("0123456789")!=string::npos) return 2;
            counts[n++]=stoi(token);
        }
    } else {
        if(tuple.size()>7 || tuple.find_first_not_of("0123456789")!=string::npos) return 2;
        n=tuple.size();
        for(int i=0;i<n;i++) counts[i]=tuple[i]-'0';
    }
    if(n<2||n>7||!is_sorted(counts,counts+n)||counts[0]<1||accumulate(counts,counts+n,0)>24)return 2;
    for(int k=1;k<n;k++){
        array<int,7> p{};for(int j=0;j<k;j++)p[j]=j;
        do{orders[k].push_back(p);}while(next_permutation(p.begin(),p.begin()+k));
    }
    string first(counts[0],'0');nodes[1]=1;
    Placement initial(first,1);bool ok=initial.run();
    cout<<"profile "<<tuple<<" witness "<<(ok?witness:"NONE")<<" actual final cores "<<calls<<endl;
    for(int k=1;k<n;k++)cout<<"level "<<k<<" unique "<<nodes[k]<<" raw "<<raw[k]<<endl;
}
