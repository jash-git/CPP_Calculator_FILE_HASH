#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std;

/*
資料來源:https://blog.csdn.net/luo_xianming/article/details/24500243
驗證:https://md5file.com/calculator
*/

unsigned long long data[0x2000000];//载入数据、
unsigned long long T1,T2,W[80];//本轮使用的消息，和分组中保存的消息
unsigned long long HashI_1[8];//中间结果
unsigned long long HashI[8];//最终结果
const unsigned long long Kt[80] = {//80个常数
		0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL,
        0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
        0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL,
        0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
        0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
        0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
        0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL, 0x2de92c6f592b0275ULL,
        0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
        0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL,
        0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
        0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL,
        0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
        0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL,
        0x92722c851482353bULL, 0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
        0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
        0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
        0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL,
        0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
        0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL,
        0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
        0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL,
        0xc67178f2e372532bULL, 0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
        0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL,
        0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
        0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
        0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
        0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL,
};
void InitializeHash(){//初始化 HashI_1 HashI
	HashI[0] = 0x6a09e667f3bcc908;
	HashI[1] = 0xbb67ae8584caa73b;
	HashI[2] = 0x3c6ef372fe94f82b;
	HashI[3] = 0xa54ff53a5f1d36f1;
	HashI[4] = 0x510e527fade682d1;
	HashI[5] = 0x9b05688c2b3e6c1f;
	HashI[6] = 0x1f83d9abfb41bd6b;
	HashI[7] = 0x5be0cd19137e2179;
}
unsigned long long ROTR(unsigned long long x,int n)//循环右移n位
{
	return ((x>>n) | (x<<(64-n))) ;
}
void SHA_512(int N){
	int i,t,j;
	for(i =0; i < N;i++){
		for(j =0; j < 16;j++)
			W[j] = data [ i*16 +j];//从全部数据中载入本次所需的消息
		for(j = 16;j < 80;j++)//将16-79轮的信息计算出
			W[j] = (ROTR(W[j-2],19) ^ ROTR(W[j-2],61) ^ (W[j-2]>>6) ) + W[j-7] + ( ROTR(W[j-15],1) ^ ROTR(W[j-15],8) ^ (W[j-15]>>7)) + W[j-16];
		for(j = 0;j<8;j++)
			HashI_1[j] = HashI[j];//每次开始输入之前，将之前得到的输出读入，然后对中间的hashI_1值进行操作，输出给HashI
		for(t =0 ;t < 80 ;t++){//80轮操作
			T1 = HashI_1[7] + ((HashI_1[4] & HashI_1[5]) ^ ( (~HashI_1[4]) & HashI_1[6]))
				+ (ROTR(HashI_1[4],14) ^ ROTR(HashI_1[4],18) ^ ROTR(HashI_1[4],41)) +W[t] + Kt[t];
			T2 = (ROTR(HashI_1[0],28) ^ ROTR(HashI_1[0],34) ^ ROTR(HashI_1[0],39))
				+ ((HashI_1[0] & HashI_1[1]) ^ (HashI_1[0] & HashI_1[2]) ^ (HashI_1[1] & HashI_1[2]));


			HashI_1[7] = HashI_1[6];
			HashI_1[6] = HashI_1[5];
			HashI_1[5] = HashI_1[4];
			HashI_1[4] = HashI_1[3] + T1;
			HashI_1[3] = HashI_1[2];
			HashI_1[2] = HashI_1[1];
			HashI_1[1] = HashI_1[0];
			HashI_1[0] = T1 + T2;
		}
		for(j = 0;j < 8;j++)
			HashI[j] += HashI_1[j];//得到输出
	}
}
void pause()
{
    printf("Press Enter key to continue...");
    fgetc(stdin);
}
int main(int argc, char **argv)
{
    if(argc>1)
    {
        int i,t,k ,l,j=0;
        int N ,M;//n个1024,M个256m
        unsigned char lastChar[128];
        unsigned long long TxtLength;
        unsigned long long r;
        InitializeHash();
        fstream dataf(*(argv+1),ios::in | ios::binary );
        dataf.seekp(0,ios::end);
        TxtLength = dataf.tellp();//获得文件的大小
        dataf.seekp(0,ios::beg);
        N = 1<<21;//256m中 含有 1^21个的1024
        M = (TxtLength>>28) + 1;//获得数据有多少个256m的块
        for( t = 0; t < M;t++){
            if( t == M-1){
                N =(TxtLength - (1<<28)*(M-1)) >>7 ;//当只剩下最后一组256m时，计算剩下的1024组数-1
                for( i =0;i< N;i++){//将剩下的满1024的组先读入
                    dataf.read((char*)lastChar,128);//一次读取128个char
                    for(k = 0;k<16;k++){
                        data[j] = 0;
                        for(l = 0;l < 8;l++)
                            data[j] = (data[j]<<8) | lastChar[k*8 + l];
                        j++;
                    }
                }
                N = TxtLength - (1<<28)*(M-1) - (N<<7);//计算最后剩下的字节数
                for( i = 0;i < N;i++)
                    dataf.read((char*)(&lastChar[i]),1);
                if( i >= 112){//补余时，若最后一段大于896则必须再加一层1024.
                    lastChar[i++] = 128;//最高位填充1之后填充0
                    for(;i < 128;i++)
                        lastChar[i] = 0;
                    for(i = 0;i < 16;i++){
                        data[j] = 0;
                        for(k = 0;k < 8;k++)
                            data[j] = (data[j]<<8) | lastChar[i*8 + k];
                    j++;
                    }
                    for(i=0;i < 112;i++)//新的1024行要再次填充到896位
                        lastChar[i] = 0;
                }else{
                    lastChar[i++] = 128;//最高位填充1之后填充0
                    for(;i < 112;i++)
                        lastChar[i] = 0;
                }
                //最后128位为消息长度，第一个数固定为0，第二个数直接为TextLength * 8
                //将数据从lastChar数组中读入到data数组中
                for(i = 0;i < 14;i++){
                    data[j] = 0;
                    for(k = 0;k < 8;k++)
                        data[j] = (data[j]<<8) | lastChar[i*8 + k];
                    j++;
                }
                data[j++] = 0;
                data[j++] = TxtLength<<3 ;
                N = j>>4;//j的数量肯定是合理的，则可以由此时j的数量得到最后1024的组数
                SHA_512(N);//进行hash
            }else{
                for( i =0;i<N;i++){
                    dataf.read((char*)lastChar,128);
                    for(k = 0;k<16;k++){
                        data[j] = 0;
                    for(l = 0;l < 8;l++)
                        data[j] = (data[j]<<8) | lastChar[k*8 + l];
                    j++;
                    }
                }
                SHA_512(N);//
            }
        }
        dataf.close();

        cout<<"SHA-512 ANS:"<<endl;
        for(j = 0; j < 8;j++)
            printf("%016I64x",HashI[j]);//printf("%016I64x\n",HashI[j]);

        printf("\n");
    }
    else
    {
        cout <<"Not input file\n";
    }


	pause();
	return 0;
}
