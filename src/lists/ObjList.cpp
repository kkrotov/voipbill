#include "Obj.h"
#include "ObjList.h"
#include "../classes/DbException.h"
#include <stdlib.h>

void _CallObj::make_dt() {
	parseDateTime((char*)&time, dt);
}

ObjList::ObjList(){
    this->data = 0;
    this->count = 0;
    this->loaded = false;
}
ObjList::ObjList(const ObjList & w){
    this->i_size = w.i_size;
    this->count = w.count;
    this->last_use = w.last_use;
    this->loadtime = w.loadtime;
    this->loadsize = w.loadsize;
    this->loaded = w.loaded;
    this->data = malloc( w.loadsize );
    memcpy(this->data, w.data, w.loadsize);
}

ObjList::~ObjList(){
    if (data != 0) free(data);
}

void ObjList::load(BDb * db){
    load(db, 0);
}

void ObjList::load(BDb * db, time_t dt){
    this->dt = dt;

    t.start();

    BDbResult res = db->query(sql(db));

    i_size = item_size();
    count = res.size();
    loadsize = count * i_size;
    loadtime = time(NULL);
    last_use = loadtime;

    if (data != 0) { free(data); data = 0; }
    if (count > 0) {
        data = malloc( loadsize );

        void * pdata = data;
        while(res.next()){
            parse_item(res, pdata);
            pdata = (char *)pdata + i_size;
        }
    }
    this->loaded = true;

    t.stop();
}
void ObjList::pre_free(){
    if (data != 0) {
        free(data);
        data = 0;
    }
}

void * ObjList::_get(size_t i){
    return (char *)data + i*i_size;
}

void * ObjListByPrefix::_find(char * prefix)
{
    int lowlow = 0, highhigh = count - 1;

    void * res_item = 0;

    int lenphone = strlen(prefix);
    for(int i=0;i<lenphone;i++){
        int low = lowlow, high = highhigh;
        char str[21];	char *pstr = &str[0];
        char str2[21];	char *pstr2 = &str2[0];
        memcpy(pstr, prefix, i+1);
        str[i+1] = 0;
        while( low <= high )
        {
            int mid = ( low + high ) / 2;

            void * item = (char*)data + mid*i_size;
            char * key_prefix = key(item);

            int diff = strcmp(key_prefix, pstr);
            if( diff < 0 )
                low = mid + 1;
            else if( diff > 0 )
                high = mid - 1;
            else {
                res_item = item;
                break;
            }

            memcpy(pstr2, key_prefix, i+1);
            str2[i+1]=0;
            diff = strcmp(pstr2, pstr);
            if( diff < 0 )
                lowlow = mid + 1;
            else if( diff > 0 )
                highhigh = mid - 1;

        }
    }
    return res_item;

}

void * ObjListByIntPrefix::_find(int value0, char * prefix)
{
    int lowlow = 0, highhigh = count - 1;

    void * res_item = 0;

    int lenphone = strlen(prefix);
    for(int i=0;i<lenphone;i++){
        int low = lowlow, high = highhigh;
        char str[21];	char *pstr = &str[0];
        char str2[21];	char *pstr2 = &str2[0];
        memcpy(pstr, prefix, i+1);
        str[i+1] = 0;
        while( low <= high )
        {
            int mid = ( low + high ) / 2;

            void * item = (char*)data + mid*i_size;
            int key0_value = key0(item);
            char * key_prefix = key(item);


            if (key0_value < value0){
                low = mid + 1;
                lowlow = low;
                continue;
            }else if (key0_value > value0){
                high = mid - 1;
                highhigh = high;
                continue;
            }

            int diff = strcmp(key_prefix, pstr);
            if( diff < 0 )
                low = mid + 1;
            else if( diff > 0 )
                high = mid - 1;
            else {
                res_item = item;
                break;
            }

            memcpy(pstr2, key_prefix, i+1);
            str2[i+1]=0;
            diff = strcmp(pstr2, pstr);
            if( diff < 0 )
                lowlow = mid + 1;
            else if( diff > 0 )
                highhigh = mid - 1;

        }
    }
    return res_item;

}

void * ObjListByInt::_find(int value)
{
    int low = 0, high = count - 1;
    while( low <= high )
    {
        int mid = ( low + high ) / 2;

        void * item = (char*)data + mid*i_size;
        int key_value = key(item);

        if( key_value < value )
            low = mid + 1;
        else if( key_value > value )
            high = mid - 1;
        else
            return item;
    }
    return 0;
}

void * ObjListBy2Long::_find(long long int value)
{
    int low = 0, high = count - 1;
    while( low <= high )
    {
        int mid = ( low + high ) / 2;

        void * item = (char*)data + mid*i_size;
        long long int key_value = key(item);

        if( key_value < value )
            low = mid + 1;
        else if( key_value > value )
            high = mid - 1;
        else
            return item;
    }
    return 0;
}

