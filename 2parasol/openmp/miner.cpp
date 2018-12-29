/*
 *
 * PARASOL based on the ordinary entry table
 * made by Yoshitaka Yamamoto
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <iterator>
#include <chrono>
#include <immintrin.h>
//#include <omp.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <sys/resource.h>


inline size_t hashfun(const std::vector<int32_t> &v) {
    int32_t h = 0;
    int primes[7] = {1,547,1229,1993,2749,3581,4421};
    auto N= v.size();
    for(auto i = 0; i < N ; i++){
        h += (v[i] * primes[i%7]);
    }
    return static_cast<size_t>(h);
}





struct hashfun_charp {
    size_t operator()(const std::vector<int32_t> &v) const {
        return hashfun(v);
    }
};



struct equal_charp{
    bool operator()(const std::vector<int32_t>& lhs ,const std::vector<int32_t>& rhs) const{
        if(lhs.size()!=rhs.size())return false;
        auto rit = rhs.begin();
        for(auto it = lhs.begin(); it!=lhs.end() ; it++){
            if(*it!=*rit)return false;
            rit++;
        }
        return true;
    }
};


void parseLine(const std::string &line , std::vector<int32_t> &vecs){
    std::istringstream iss(line);
    std::string tmp;
    while(getline(iss , tmp , ' ')){
        if(tmp.size()==0)break;
        vecs.push_back( std::stoi(tmp) );
    }
}

void parseLine_fast(const std::string &line , std::vector<int32_t> &vecs){
    char *beg = (char*)line.c_str();
    char *end = beg+line.size();
    char *cur;
    int32_t val = 0;
    bool flag = false;
    for(cur = beg ; cur!=end ; cur++){
        assert(*cur!='-' && *cur!='+');
        if(*cur==' ' && flag){
            vecs.push_back( val );
            val = 0;
            flag = false;
            continue;
        }
        flag = true;
        val = val*10 + (*cur - '0');
    }
    if(flag)vecs.push_back( val );
}

size_t intersect_int32_fast(const int32_t *A, const size_t s_a,const int32_t *B, const size_t s_b, int32_t *out) {
    assert(out != A);
    assert(out != B);
    size_t count = 0;
    size_t i_a = 0, i_b = 0;
    size_t a = 0;
    size_t b = 0;
    while(i_a < s_a && i_b < s_b)
    {
        a = A[i_a];
        b = B[i_b];
        if(a == b){
            out[count] = a;
            count++;
            i_a++;
            i_b++;
        }
        else if(a > b){
            i_b++;
        }
        else {
            i_a++;
        }
    }
    return count;
}

class Score{
public:
    size_t t_count;
    size_t delta_count;
    size_t index;
    Score(){
        this->t_count = 0;
        this->delta_count = 0;
    }
    Score(size_t v1 , size_t v2){
        this->t_count = v1;
        this->delta_count = v2;
    }
};

typedef struct{
    size_t t_count;
    size_t delta_count;
    std::vector<int32_t> subseq;
} NewEntry;


template <class T>
size_t saveSpace(T &cntmap, size_t max_count, size_t min_count, size_t max_size){
    if(cntmap.size() <= max_size){
        return min_count;
    }
    size_t delta = min_count + 1;
    std::vector<size_t> cntdist(max_count-delta+1 , 0);
    
    for(int b=0;b<cntmap.bucket_count();b++){
        for(auto it = cntmap.begin(b); it!=cntmap.end(b) ; it++){
            //cout << test_i << endl;
            //	   test_i++;
            const Score& val = it->second;
            int est_count = val.t_count + val.delta_count;
            assert(est_count>=delta);
            size_t offset = static_cast<size_t>(est_count - delta);
            if(offset<cntdist.size()){
                cntdist[offset]++;
            }
        }
    }
    
    if(cntmap.size()>max_size){
        size_t Ncnt = 0;
        for(int j = 0 ; j<cntdist.size() ; j++){
            Ncnt += cntdist[j];
            if(Ncnt >= cntmap.size() - max_size){
                delta = delta + j;
                break;
            }
        }
        for(auto it=cntmap.begin() ; it!=cntmap.end() ; ){
            const Score& val = it->second;
            int est_count = val.t_count + val.delta_count;
            if(est_count <= delta){
                it = cntmap.erase(it);
            }else{
                ++it;
            }
        }
    }
    
    return delta;
}



int main(int argc,char** argv){
    std::unordered_map<std::vector<int32_t> , Score , hashfun_charp , equal_charp> cntmap;
    std::vector<int32_t> itemset;
    std::string line;
    size_t Kmax = 10000;   /* size constant */
    size_t error = 0;    /* error parameter */
    size_t current_delta = 0;
    size_t line_count = 0;
    size_t top_count = 0;
    double total_ratio_nonempty = 0;
    NewEntry *candidates;
    std::chrono::system_clock::time_point t0,t1,tx,ty,tz,tw,tv,t_start;
    std::chrono::duration<double> time_prepare(0.0);
    std::chrono::duration<double> time_intersect(0.0);
    std::chrono::duration<double> time_update(0.0);
    std::chrono::duration<double> time_remove(0.0);
    t_start = std::chrono::system_clock::now();
    t0 = t_start;
    
    std::ios::sync_with_stdio(false);
    if(argc==2){
        Kmax = std::stoi(argv[1]);
    }
    else if(argc==3){
        Kmax = std::stoi(argv[1]);
        error = std::stod(argv[2]);
    }
    candidates = new NewEntry[Kmax+1];
    while(std::getline(std::cin , line)){
        tv  = std::chrono::system_clock::now();
        /* space saving */
        if(cntmap.size()>Kmax){
            current_delta = saveSpace(cntmap, top_count, current_delta, Kmax);
            assert(cntmap.size()<=Kmax);
            {
                tw = std::chrono::system_clock::now();
                auto diff0 = tw-tv;
                time_remove += diff0;
            }
            int idx=0;
            // space saving
            for(auto it=cntmap.begin(); it!=cntmap.end() ; it++){
                it->second.index = idx;
                ++idx;
            }
        }
        
        /* read input */
        itemset.clear();
        parseLine_fast(line , itemset);
        std::sort(itemset.begin() , itemset.end());
        /* remove duplicates */
        itemset.erase(std::unique(itemset.begin(),itemset.end()),itemset.end());
        tx  = std::chrono::system_clock::now();
        time_prepare += (tx-tv);
        tx  = std::chrono::system_clock::now();
        
        if(itemset.size()==0) continue;
        line_count+=1;
        //for(auto it=itemset.begin(); it!=itemset.end(); it++){
          //  std::cout << *it << " ";
        //}
        //std::cout << std::endl;
        int total_nonempty = 0;

        int32_t *tmp = (int32_t*)malloc(sizeof(int32_t)*(itemset.size()+4));
        for(int b=0;b<cntmap.bucket_count();b++){
            for(auto it = cntmap.begin(b); it!=cntmap.end(b) ; it++){
                size_t sz;
                const std::vector<int> &oldset = it->first;
                size_t oldcnt = it->second.t_count;
                size_t old_delta = it->second.delta_count;
                size_t idx = it->second.index;
                sz = intersect_int32_fast(itemset.data() , itemset.size() ,
                                          oldset.data() , oldset.size() ,
                                          tmp);
                
                candidates[idx].subseq.clear();
                
                if(sz==oldset.size()){
                    total_nonempty++;
                    it->second.t_count+=1;
                }else if(sz>0){
                    total_nonempty++;
                    std::copy(tmp , tmp+sz , std::back_inserter(candidates[idx].subseq));
                    if(cntmap.find(candidates[idx].subseq)==cntmap.end()){
                        candidates[idx].t_count = oldcnt+1;
                        candidates[idx].delta_count = old_delta;
                    }else{
                        candidates[idx].subseq.clear();
                    }
                }
            }
        }
        if((int)cntmap.size() != 0){
            total_ratio_nonempty += (double)(total_nonempty/(int)cntmap.size());
        }
        std::cout << "size" << (int)cntmap.size() <<  " non_empty : " << total_nonempty << "total_ratio : " << total_ratio_nonempty << std::endl;
        
        free(tmp);
        /* end of intersection */
        auto idx = cntmap.size();
        candidates[idx].subseq.clear();
        std::copy(itemset.begin() , itemset.end() , std::back_inserter(candidates[idx].subseq));
        candidates[idx].t_count = 1;
        candidates[idx].delta_count = current_delta;
        ty  = std::chrono::system_clock::now();
        time_intersect += (ty-tx);
        /* register new entry */
        size_t max_idx = cntmap.size();
        for(auto i=0;i<idx+1;i++){
            if(candidates[i].subseq.size()==0)continue;
            
            size_t newcnt = candidates[i].t_count;
            size_t new_delta = candidates[i].delta_count;
            top_count = std::max(top_count , newcnt+new_delta);
            auto pos = cntmap.find(candidates[i].subseq);
            if(pos!=cntmap.end()){
                size_t oldcnt = pos->second.t_count;
                size_t old_delta = pos->second.delta_count;
                if(newcnt>oldcnt){
                    pos->second.t_count = newcnt;
                    pos->second.delta_count = new_delta;
                }else if(newcnt==oldcnt && oldcnt+old_delta < newcnt+new_delta ){
                    pos->second.t_count = newcnt;
                    pos->second.delta_count = new_delta;
                }
            }else{
                Score newent(newcnt , new_delta);
                newent.index = max_idx;
                max_idx++;
                cntmap[candidates[i].subseq] = newent;
            }
        }
    }
    tz  = std::chrono::system_clock::now();
    time_update += (tz-ty);
    /* end of main loop */
    /* output result */
    
    for(auto it=cntmap.begin(); it!=cntmap.end(); it++){
        const std::vector<int32_t>& keyset = it->first;
        std::cout << "[ " ;
        for(int32_t n : keyset){
            std::cout << n << " ";
        }
        std::cout << "]," << it->second.t_count <<", " << (it->second.delta_count+it->second.t_count) << std::endl;
    }
    /* output information */
    struct rusage usage;
    auto chk = getrusage(RUSAGE_SELF, &usage);
    char *s_omp_num_threads = getenv("OMP_NUM_THREADS");
    char *s_omp_schedule = getenv("OMP_SCHEDULE");
    t1 = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = t1 - t_start;
    std::cerr << "delta=" << current_delta << std::endl;
    std::cerr << "k=" << Kmax << std::endl;
    if(s_omp_num_threads!=NULL){
        std::cerr << "OMP_NUM_THREADS=" << s_omp_num_threads << std::endl;
    }else{
        std::cerr << "OMP_NUM_THREDAS=1"<<std::endl;
    }
    
    if(s_omp_schedule!=NULL){
        std::cerr << "OMP_SCHEDULE=" << s_omp_schedule << std::endl;
    }else{
        std::cerr << "OMP_SCHEDULE=" << "STATIC" << std::endl;
    }
    std::cerr << "total time = " << diff.count() << "[sec]" << std::endl;
    std::cerr << "time to prepare = " << time_prepare.count() << "[sec]"<<std::endl;
    std::cerr << "time to remove = " << time_remove.count() << "[sec]" << std::endl;
    std::cerr << "time to intersect =  " << time_intersect.count() << "[sec]" << std::endl;
    std::cerr << "time to update = " << time_update.count() << "[sec]" << std::endl;
    std::cerr << "time per line = " << 1000.0*diff.count()/line_count << "[msec]" << std::endl;
    if(chk==0){
        std::cerr << "Max RSS = " << usage.ru_maxrss/1024.0 << "[MB]" << std::endl;
    }
    
    delete [] candidates;
    return 0;
}

