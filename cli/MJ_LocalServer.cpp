#include "MJ_LocalServer.h"

static MJ_Base::CARD cardSet[] = {
    'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',//��
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',//��
    'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',//Ͳ
    'A', 'B', 'C', 'D', 'E', 'F', 'G',          //��

    'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
    'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G',

    'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
    'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G',

    'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
    'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G'
};

MJ_LocalServer::MJ_LocalServer(QObject *parent) : QObject(parent)
{
    for(auto i=0; i<4; i++)
        this->member[i] = new MJ_Player();

    cur_id = 0;
    zhuang_id = -1;
    paiCount = 0;
    maxPaiCount = 136;

    memset(this->paiList, 0, this->paiCount);

    stat = this->SVR_normal;//����ģʽ
    tmOut = new QTimer(this);
    connect(this->tmOut, SIGNAL(timeout()), this, SLOT(tmOutSlot()), Qt::QueuedConnection);
}

MJ_LocalServer::~MJ_LocalServer()
{
    delete this->member[0];
    delete this->member[1];
    delete this->member[2];
    delete this->member[3];
}

void MJ_LocalServer::send(MJ_response &resp)
{
    emit responseSignal(resp);
}

//���֣�ѡ����ѡׯ�����ƣ���ʼ����13�ţ���
void MJ_LocalServer::start()
{
    MJ_Base::CARD cards[4][16] = {0};
    maxPaiCount = 136;
    wj_ready = 0;//��һ�û���������ϵ���

    srand(time(NULL));

    //if(zhuang_id < 0)  // ��һ��
    //    this->zhuang_id = rand() % 4; // ѡׯ
    this->zhuang_id = 0;
    cur_id = zhuang_id;//��ǰ������

    //ϴ��
    int cardCount[136] = {0};
    for(auto i=0; i<136; i++)
    {
        int r = rand() % 136;
        while(cardCount[r] != 0) r++, r %= 136;

        paiList[i] = cardSet[r];
        cardCount[r] = 1;//�����ѡ
    }
    //ѡ��
    shaizi1 = rand()%6+1;
    shaizi2 = rand()%6+1;

    maxPaiCount -= (shaizi1 + shaizi2) * 2;
    this->wang = paiList[maxPaiCount];
    switch(this->wang)
    {
    case MJ_Base::MJ_JIUWAN:
        this->wang = MJ_Base::MJ_YIWAN;
        break;
    case MJ_Base::MJ_JIUTONG:
        this->wang = MJ_Base::MJ_YITONG;
        break;
    case MJ_Base::MJ_JIUTIAO:
        this->wang = MJ_Base::MJ_YITIAO;
        break;
    case MJ_Base::MJ_BAN:
        this->wang = MJ_Base::MJ_DONG;
        break;
    default:
        this->wang += 1;
    }

    //����
    for(int i=0; i<13; i++)
    {
        cards[0][i] = this->paiList[paiCount++];
        cards[1][i] = this->paiList[paiCount++];
        cards[2][i] = this->paiList[paiCount++];
        cards[3][i] = this->paiList[paiCount++];
    }

    // ���Ϳ�����Ϣ
    MJ_response resp;
    resp.setCard(this->wang);
    resp.setPaiList(cards[0]);
    resp.setSendTo(0);
    resp.setType(MJ_response::T_Init);
    this->send(resp);

    resp.setCard(this->wang);
    resp.setPaiList(cards[1]);
    resp.setSendTo(1);
    resp.setType(MJ_response::T_Init);
    this->send(resp);

    resp.setCard(this->wang);
    resp.setPaiList(cards[2]);
    resp.setSendTo(2);
    resp.setType(MJ_response::T_Init);
    this->send(resp);

    resp.setCard(this->wang);
    resp.setPaiList(cards[3]);
    resp.setSendTo(3);
    resp.setType(MJ_response::T_Init);
    this->send(resp);
}

void MJ_LocalServer::faPai()
{
    this->card = this->paiList[paiCount++];

    MJ_response resp;
    resp.setSendTo(cur_id);
    resp.setType(MJ_response::T_FaPai);
    resp.setCard(this->card);

    send(resp);
}

void MJ_LocalServer::resl_chuPai(MJ_RequestData &req)
{
    int senderID = req.getSenderID();
    if(senderID >= 4 || senderID < 0)
        return ;

    MJ_Base::CARD cd = req.getCard();
    MJ_response resp;
    resp.setType(MJ_response::T_Ok);
    resp.setSendTo(senderID);
    send(resp);

    cur_id = senderID;
    int j = senderID + 1;
    j %= 4;//��λ���¼�

    mem_policy[j] = P_None;
    if(member[j]->testHu(cd))
        mem_policy[j] |= (int)P_Hu;
    if(member[j]->testGang(cd))
        mem_policy[j] |= P_Gang;
    if(member[j]->testPeng(cd))
        mem_policy[j] |= P_Peng;
    if(member[j]->testChi(cd))
        mem_policy[j] |= P_Chi;
    if(mem_policy[j] != P_None) // ��ѡ��
    {
        this->stat = SVR_vote;//�ȴ�����
    }

    j += 1; j %= 4;
    mem_policy[j] = P_None;
    if(member[j]->testHu(cd))
        mem_policy[j] |= P_Hu;
    if(member[j]->testGang(cd))
        mem_policy[j] |= P_Gang;
    if(member[j]->testPeng(cd))
        mem_policy[j] |= P_Peng;
    if(mem_policy[j] != P_None) // ��ѡ��
    {
        this->stat = SVR_vote;//�ȴ�����
    }

    j += 1; j %= 4;
    mem_policy[j] = P_None;
    if(member[j]->testHu(cd))
        mem_policy[j] |= P_Hu;
    if(member[j]->testGang(cd))
        mem_policy[j] |= P_Gang;
    if(member[j]->testPeng(cd))
        mem_policy[j] |= P_Peng;
    if(mem_policy[j] != P_None) // ��ѡ��
    {
        this->stat = SVR_vote;//�ȴ�����
    }

    //֪ͨ������ң�ĳ��ҳ���һ����
    resp.setType(MJ_response::T_ChuPai);
    resp.setCard(cd);
    resp.setWho(senderID);
    resp.setSendTo(MJ_response::SDT_Broadcast);
    send(resp);

    if(this->stat == SVR_vote) // ��������ҿ��� ��������
    {
        current_policy = P_None;
        resp.setType(MJ_response::T_Wait);
        resp.setWho(senderID);
        resp.setSendTo(MJ_response::SDT_Broadcast);
        send(resp);

        this->tmOut->start(10000);//10s ѡ��ʱ��
        f_HGPC_valid = true;
    }
    else if(this->stat == SVR_normal) // ��������
    {
        cur_id ++;
        cur_id %= 4;
        this->faPai();
    }
    else   // �����ˣ�
    {
        // do print err log
    }
}

void MJ_LocalServer::resl_HGPCList(MJ_RequestData &request)
{
    int senderID = request.getSenderID();
    if(senderID >= 4 || senderID < 0)
        return ;

    if(wj_ready & 0x0f != 0x0f)
    {
        wj_ready |= senderID + 1;
    }

    MJ_Base::CARD hu[8] = {0};
    MJ_Base::CARD g[8] = {0};
    MJ_Base::CARD p[8] = {0};
    MJ_Base::CARD c[8] = {0};

    request.getHGPCList(hu, g, p, c);

    member[senderID]->setcChiList(c);
    member[senderID]->setcPengList(p);
    member[senderID]->setcGangList(g);
    member[senderID]->setcHuList(hu);
}

void MJ_LocalServer::resl_Hu(MJ_RequestData &req)
{
    int senderID = req.getSenderID();

    if(!f_HGPC_valid)
        return;

    if(this->mem_policy[senderID] & P_Hu)
    {
        if(this->current_policy != P_None)
        {
            if(this->current_policy == P_Gang)
            {

            }
            else if(this->current_policy == P_Peng)
            {

            }
            else if(this->current_policy == P_Chi)
            {

            }
            else//���⣡
            {

            }
        }

        this->stat = SVR_normal;
        if(this->tmOut->isActive())
            this->tmOut->stop();

        MJ_response resp;
        resp.setType(MJ_response::T_Hu);
        resp.setCard(req.getCard());
        resp.setWho(senderID);
        resp.setSendTo(MJ_response::SDT_Broadcast);
        //���ֽ���
    }
    else
    {

    }
}

void MJ_LocalServer::resl_Gang(MJ_RequestData &req)
{
     int senderID = req.getSenderID();

     if(!f_HGPC_valid)
         return;

     if(current_policy > P_Gang)
         return ;

     if(mem_policy[senderID] & P_Gang)
     {
         if(current_policy == P_Peng)
         {

         }
         else if(current_policy == P_Chi)
         {

         }
         else //���⣡
         {

         }

         MJ_Base::CARD cd = req.getCard();
         current_policy = P_Gang;
         mem_policy[current_policy_ID] = P_None;//ȡ����״̬

         MJ_response resp;
         resp.setType(MJ_response::T_Gang);
         resp.setCard(cd);
         resp.setSendTo(MJ_response::SDT_Broadcast);
         resp.setWho(senderID);
         send(resp);

         if(mem_policy[0] <= P_Gang && mem_policy[1] <= P_Gang &&
                 mem_policy[2] <= P_Gang && mem_policy[3] <= P_Gang)
         {
             this->stat = SVR_normal;
             if(this->tmOut->isActive())
                 this->tmOut->stop();

             resp.setType(MJ_response::T_ChuPai);
             resp.setSendTo(MJ_response::SDT_Broadcast);
             resp.setWho(senderID);

             send(resp);
         }
     }
}

void MJ_LocalServer::resl_Peng(MJ_RequestData &req)
{
    int senderID = req.getSenderID();

    if(!f_HGPC_valid)
        return;

    if(current_policy > P_Peng)
        return ;

    if(mem_policy[senderID] & P_Peng)
    {
        // ����ҳ��������ƣ����˲���
        if(current_policy == P_Chi)
        {

        }
        else //���⣡
        {

        }

        MJ_Base::CARD cd = req.getCard();
        current_policy = P_Peng;
        mem_policy[current_policy_ID] = P_None;//ȡ����״̬

        MJ_response resp;
        resp.setType(MJ_response::T_Peng);
        resp.setCard(cd);
        resp.setSendTo(MJ_response::SDT_Broadcast);
        resp.setWho(senderID);
        send(resp);

        if(mem_policy[0] <= P_Peng && mem_policy[1] <= P_Peng &&
                mem_policy[2] <= P_Peng && mem_policy[3] <= P_Peng)
        {
            this->stat = SVR_normal;
            if(this->tmOut->isActive())
                this->tmOut->stop();

            resp.setType(MJ_response::T_ChuPai);
            resp.setSendTo(MJ_response::SDT_Broadcast);
            resp.setWho(senderID);

            send(resp);
        }
    }
}

void MJ_LocalServer::resl_Chi(MJ_RequestData &req)
{
    int senderID = req.getSenderID();

    if(!f_HGPC_valid)
        return;

    if(current_policy > P_Chi)
        return;

    // ֻ�ܳ��ϼҵ���
    if(current_policy == P_None && (cur_id+1)%4 == senderID)
    {
        req.getChi(this->chi);

        current_policy_ID = senderID;
        current_policy = P_Chi;
        mem_policy[current_policy_ID] = P_None;//ȡ����״̬

        //�㲥�����ҵĳԲ���
        MJ_response resp;
        resp.setType(MJ_response::T_Chi);
        resp.setWho(senderID);
        resp.setChi(this->chi);
        resp.setSendTo(MJ_response::SDT_Broadcast);
        send(resp);

        // ���û�бȳԸ������ȼ��Ĳ������ڣ������ҳ���
        if(mem_policy[0] <= P_Chi && mem_policy[1] <= P_Chi &&
                mem_policy[2] <= P_Chi && mem_policy[3] <= P_Chi)
        {
            this->stat = SVR_normal;
            if(this->tmOut->isActive())
                this->tmOut->stop();

            resp.setType(MJ_response::T_ChuPai);
            resp.setSendTo(MJ_response::SDT_Broadcast);
            resp.setWho(senderID);

            send(resp);
        }
    }
    else // ��Ӧ���ɹ�
    {

    }
}

void MJ_LocalServer::resl_Cancel(MJ_RequestData &req)
{
    int senderID = req.getSenderID();

    this->mem_policy[senderID] = P_None;//ȡ���������
    MJ_response resp;


    // �������Ҳ���ȡ����֮ǰ�����ѡ��Ĳ����ǵ�ǰ���ֵʱ
    if(mem_policy[0] <= current_policy && mem_policy[1] <= current_policy &&
            mem_policy[2] <= current_policy && mem_policy[3] <= current_policy)
    {
        //  û�в�������ִ�������֧�Ļ������Է��Ƹ��¼���
        if(current_policy == P_None)
        {
            cur_id++;
            cur_id %= 4;
            this->faPai();
        }

        this->stat = SVR_normal;
        if(this->tmOut->isActive())
            this->tmOut->stop();

        resp.setType(MJ_response::T_ChuPai);
        resp.setSendTo(MJ_response::SDT_Broadcast);
        resp.setWho(this->current_policy_ID);//

        send(resp);
    }
}

void MJ_LocalServer::RecvSlot(MJ_RequestData request)
{
    int type = request.getType();
//    int senderID = request.getSenderID();

    switch(type)
    {
    case MJ_RequestData::R_Init:
        this->start();
        break;

    case MJ_RequestData::R_HGPCList:
        this->resl_HGPCList(request);
        if(wj_ready &  0x0f == 0x0f)
            this->faPai();
        break;

    case MJ_RequestData::R_ChuPai:
        this->resl_chuPai(request);
        break;

    case MJ_RequestData::R_Hu:
        break;

    case MJ_RequestData::R_Gang:
        break;

    case MJ_RequestData::R_Peng:
        break;

    case MJ_RequestData::R_Chi:
        break;
    case MJ_RequestData::R_CanCel:
        break;

    default: // �����ˣ�
        break;
    }
}

void MJ_LocalServer::tmOutSlot()
{

}





