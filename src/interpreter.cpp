#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <cinttypes>
#include <cstdlib>

using namespace std;

const int BLOCK_SIZE = 4096;
const int N_BLOCKS = 4096;

const uint8_t b64[256] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,62,0,0,0,63,52,53,54,55,56,57,58,59,60,61,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,0,0,0,0,0,0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
inline int get64(const string& s, int& i){
    int res = b64[(int)s[i]];
    while(s[++i]!=(int)';'){ //terminating char for b64 number
        res=(res<<6)+b64[(int)s[i]];
    }
    ++i; //align with next readable thing
    return res;
}

int * vars[N_BLOCKS];
int   blockn;


inline void exit(){
    for(int i=0; i<blockn; ++i){
		free(vars[i]);
    }
}

inline int& var(int id){
    while(blockn*BLOCK_SIZE<id){
        vars[blockn++] = (int *)malloc(BLOCK_SIZE*sizeof(int));
	if(!vars[blockn]){
            cerr<<"Could not allocate " << blockn*BLOCK_SIZE*sizeof(int) << " bytes of memory"<<endl;
            --blockn;exit();
            throw;
        }
    }
    return vars[id/BLOCK_SIZE][id%BLOCK_SIZE];
}

//initialize
inline void init(){
    blockn = 1;
    vars[0] = (int *)malloc(BLOCK_SIZE*sizeof(int));
}

inline void readProgram(vector<string>& v, const string& filename){
    ifstream in;
    string temp;
    in.open(filename);
    while(!in.eof()){
        getline(in,temp);
        v.push_back(temp);
    }
}

struct E{
    E(char t, int v):t(t),v(v){}
    char t;
    int v;
    inline const int getVal() const{
        return (t=='c'?v:var(v));
    }
};
inline int eval(const string& s, int& i){
    stack<E> st;
    char t;
    int v;
    E o1(0,0);
    E o2(0,0);
    while(s[i]!=']'){
        t=s[i++];
        if(s[i]=='['){
            ++i;
            v=eval(s,i);
        } else {
            v=get64(s,i);
        }
        if(t=='o'){
            switch(v){
            case 0:
                o1.t='c';
                cin>>o1.v;
                st.push(o1);
                break;
            case 1: // =
                o2=st.top();st.pop();o1=st.top();st.pop();
                var(o1.v)=o2.getVal();
                st.emplace('c',o1.getVal());
                break;
            case 2: // +
                o2=st.top();st.pop();o1=st.top();st.pop();
                st.emplace('c',o1.getVal()+o2.getVal());
                break;
            case 3: // -
                o2=st.top();st.pop();o1=st.top();st.pop();
                st.emplace('c',o1.getVal()-o2.getVal());
                break;
            case 4: // *
                o2=st.top();st.pop();o1=st.top();st.pop();
                st.emplace('c',o1.getVal()*o2.getVal());
                break;
            case 5: // /
                o2=st.top();st.pop();o1=st.top();st.pop();
                st.emplace('c',o1.getVal()/o2.getVal());
                break;
            case 6: // %
                o2=st.top();st.pop();o1=st.top();st.pop();
                st.emplace('c',o1.getVal()%o2.getVal());
                break;
            case 7: // ==
                o2=st.top();st.pop();o1=st.top();st.pop();
                st.emplace('c',o1.getVal()==o2.getVal());
                break;
            case 8: // !=
                o2=st.top();st.pop();o1=st.top();st.pop();
                st.emplace('c',o1.getVal()!=o2.getVal());
                break;
            case 9: // >
                o2=st.top();st.pop();o1=st.top();st.pop();
                st.emplace('c',o1.getVal()>o2.getVal());
                break;
            case 10: // <
                o2=st.top();st.pop();o1=st.top();st.pop();
                st.emplace('c',o1.getVal()<o2.getVal());
                break;
            case 11: // >
                o2=st.top();st.pop();o1=st.top();st.pop();
                st.emplace('c',o1.getVal()>=o2.getVal());
                break;
            case 12: // <
                o2=st.top();st.pop();o1=st.top();st.pop();
                st.emplace('c',o1.getVal()<=o2.getVal());
                break;
            }
        } else if(t=='v') {
            st.emplace('v',v);
        } else if(t=='c') {
            st.emplace('c',v);
        }
    }
    ++i; //for ]
    return st.top().getVal();
}

inline void executeProgram(vector<string>& program){
    int j;
    for(int i=0; i<program.size(); ++i){
        if(program[i].size()==0)continue;
        if(program[i][0]=='['){
            j=1;
            eval(program[i],j);
        } else if(program[i][0]=='i'){
            j=1;
            int skip = get64(program[i],j);
            ++j;//for [
            if(!eval(program[i],j)){
                i+=skip;
            }
        } else if(program[i][0]=='g'){
            j=1;
            int line = get64(program[i],j);
            ++j;//for [
            i=line-1;
        } else if(program[i][0]=='p'){
            j=2;//for [
            cout<<eval(program[i],j)<<'\n';
        }
    }
}

int main(int argc, char ** argv){
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    init();
    vector<string> p;
    readProgram(p,argv[1]);
    for(unsigned int i=0; i<p.size(); ++i){
        cerr<<i<<":\t"<<p[i]<<endl;
    }
    executeProgram(p);
    exit();
    return 0;
}
