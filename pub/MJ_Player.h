#ifndef MJ_PLAYER_H
#define MJ_PLAYER_H

#include <map>
#include <string>
#include "MJ_AnalyResult.h"
#include "MJ_Base.h"

#define DEBUG

class MJ_Player : public MJ_Base
{
    friend class MJ_AnalyResult;

private:

    CARD HuList[16];
    CARD cChiList[16];
    CARD cGangList[8];
    CARD cPengList[8];

    CARD wang;

    _L cPengCount;
    _L cGangCount;
    _L cChiCount;

    std::map<CARD, MJ_AnalyResult> AnalyResults;
private:
    bool hasSanLian(CARD*li, CARD c, CARD **res);
    bool hasErLian(CARD*li, CARD c, CARD **res);
    bool hasKanZhang(CARD*li, CARD c, CARD **res);

    void MJ_sort(typename MJ_Player::CARD *a, int len);

public:

    MJ_Player();

    // 开局初始化 并 分析 本家局势
    void init(pCCARD _paiList, CARD _wang) ;

    // 把抓到的牌放入牌列
    void addCard(CARD newCard) ;

    int ChuPai(CARD c) override;
    // 删掉一张牌（出牌，吃-3， 碰-3， 杠-3）
    int DelCard(CARD card);
    int DelCard(int offset);

    // 分析可 吃碰杠胡
    // 返回值：>=0匹配结果数， <0：数据有误
    int cChi() ;
    int cPeng() ;
    int cGang() ;
    int analysis();

    int Hu(CARD c, pCCARD ll) override;
    int Gang(CARD c) override;
    int Peng(CARD c) override;
    int Chi(CARD c, pCCARD ll) override;


    //根据一张牌 获取到可以吃的牌组合。
    //返回值：-1  给出的card有误，0表示没有可以吃的，返回正整数表示res中有多少组
    //res存放结果，非线程安全
    int getCChiList(CARD card, CARD (*res)[3]);
    bool testHu(CARD c);
    bool testPeng(CARD c);
    bool testGang(CARD c);
    bool testChi(CARD c);

    int copy_chiList(MJ_AnalyResult *);
    int copy_pengList(MJ_AnalyResult *);
    int copy_gangList(MJ_AnalyResult *);


#ifdef DEBUG
    void printTest();
    void printHu();
#endif
};


#endif // MJ_Player_H
