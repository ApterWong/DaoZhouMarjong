#ifndef MJ_BASE_H
#define MJ_BASE_H

#include <string>

class MJ_Base
{
public:
    typedef int _L;
    typedef char CARD;
    typedef const char* pCCARD;

    enum MJ_CARDS{
        MJ_noCard=0,
        MJ_JIUWAN = 'r', MJ_BAWAN, MJ_QIWAN, MJ_LIUWAN, MJ_WUWAN, MJ_SIWAN, MJ_SANWAN, MJ_ERWAN, MJ_YIWAN,
        MJ_JIUTIAO = 'a', MJ_BATIAO, MJ_QITIAO, MJ_LIUTIAO, MJ_WUTIAO, MJ_SITIAO, MJ_SANTIAO, MJ_ERTIAO, MJ_YITIAO,
        MJ_JIUTONG = 'R', MJ_BATONG, MJ_QITONG, MJ_LIUTONG, MJ_WUTONG, MJ_SITONG, MJ_SANTONG, MJ_ERTONG, MJ_YITONG,
        MJ_BAN = 'A', MJ_FA, MJ_ZHONG, MJ_BEI, MJ_XI, MJ_NAN, MJ_DONG,
        MJ_WANG = '0', MJ_ANGNG='*'
    };

    enum STAT{
        ST_Ready = 1, ST_Wait, ST_OffLine
    };

protected:
    STAT stat;//玩家状态
    int x_ID;//座位号
    int p_ID;//玩家ID
    unsigned int score;//玩家分数
    int f_pri;//庄家&闲家
    std::string p_name;//玩家昵称

    int paiCount;
    int paiRecCount;

    CARD paiRecList[50];
    CARD paiList[16];
    CARD gang[8];
    CARD peng[8];
    CARD chi[16];
    CARD NewCard;

    _L _g;
    _L _p;
    _L _c;
    _L _h;
public:
    MJ_Base();

    virtual int Hu(CARD c, pCCARD ll) = 0;
    virtual int Gang(CARD c) = 0;
    virtual int Peng(CARD c) = 0;
    virtual int Chi(CARD c, pCCARD ll) = 0;

    virtual int ChuPai(CARD c) = 0;
};

#define isFENG(card) ((card) <= MJ_Player::MJ_DONG && (card) >= MJ_Player::MJ_BAN)
#define isTONG(card) ((card) >= MJ_Player::MJ_JIUTONG && (card) <= MJ_Player::MJ_YITONG)
#define isTIAO(card) ((card) >= MJ_Player::MJ_JIUTIAO && (card) <= MJ_Player::MJ_YITIAO)
#define isWAN(card) ((card) >= MJ_Player::MJ_JIUWAN && (card) <= MJ_Player::MJ_YIWAN)

//  判断是一张合法的牌
#define isMJCARD(card) (isFENG(card) || isTONG(card) || isTIAO(card) || isWAN(card))

//  三张相同的牌
#define hasSanTong(plist) ((plist)[0] == (plist)[1] && (plist)[0] == (plist)[2])
//  三连
//#define hasSanLian(plist) (!isFENG((plist)[0])) && strchr((plist), (plist)[0]-1) && strchr((plist), (plist)[0]-2)
//  对子
#define hasDuiZi(plist) ((plist)[0] == (plist)[1])
//  二连
//#define hasErLian(plist) (!isFENG((plist)[0]) && (strchr((plist), (plist)[0]-1))
//  坎张
//#define hasKanZhang(plist) (!isFENG((plist)[0]) && (strchr((plist),(plist)[0]-2)))

#endif // MJ_BASE_H
