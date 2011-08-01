/* ScriptData
SDName: boss_mimiron
SD%Complete: 75%
SDComment: Put the robot's pieces together; Implement fires in hard mode
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

/*
TODO:
implement in core
m_spellInfo->Id == 64626
m_spellInfo->Id == 65224
m_spellInfo->Id == 65192
m_spellInfo->Id == 64619*/

enum
{
    //yells
    SAY_AGGRO           = -1603241,
    SAY_HARD_MODE       = -1603242,
    SAY_BERSERK         = -1603243,
    SAY_TANK_ACTIVE     = -1603244,
    SAY_TANK_SLAY1      = -1603245,
    SAY_TANK_SLAY2      = -1603246,
    SAY_TANK_DEATH      = -1603247,
    SAY_TORSO_ACTIVE    = -1603248,
    SAY_TORSO_SLAY1     = -1603249,
    SAY_TORSO_SLAY2     = -1603250,
    SAY_TORSO_DEATH     = -1603251,
    SAY_HEAD_ACTIVE     = -1603252,
    SAY_HEAD_SLAY1      = -1603253,
    SAY_HEAD_SLAY2      = -1603254,
    SAY_HEAD_DEATH      = -1603255,
    SAY_ROBOT_ACTIVE    = -1603256,
    SAY_ROBOT_SLAY1     = -1603257,
    SAY_ROBOT_SLAY2     = -1603258,
    SAY_ROBOT_DEATH     = -1603259,

    EMOTE_PLASMA_BLAST      = -1603371,

    SPELL_JET_PACK          = 63341, // used by mimiron to change seats
    SPELL_SELF_REPAIR       = 64383,

    // hard mode spells
    SPELL_SELF_DESTRUCTION  = 64613,    // visual aura
    SPELL_SELF_DESTRUCT     = 64610,    // damage aura
    SPELL_EMERGENCY_MODE_AURA   = 65101,
    NPC_MIMIRON_INFERNO     = 33370,    // used to cast the self destruct

    SPELL_FLAMES            = 64561,    // may be the fires spells
    SPELL_FLAMES_SUMMON     = 64563,    // 64567
    SPELL_FLAMES_SPREAD     = 64562,
    NPC_FLAME               = 34121,
    NPC_FLAME_INITIAL       = 34363,

    //spells
    //leviathan
    SPELL_PROXIMITY_MINES   = 63016, // also in phase 4
    SPELL_MINE_SUMMON       = 65347,
    MOB_PROXIMITY_MINE      = 34362,
    SPELL_EXPLOSION         = 66351,
    SPELL_EXPLOSION_H       = 63009,
    SPELL_NAPALM_SHELL      = 63666,
    SPELL_NAPALM_SHELL_H    = 65026,
    SPELL_PLASMA_BLAST      = 62997,
    SPELL_PLASMA_BLAST_H    = 64529,
    SPELL_SHOCK_BLAST       = 63631,    // also in phase 4
    SPELL_FLAME_SUPRESS_MK  = 64570,    // hard mode
    LEVIATHAN_TURRET        = 34071,

    //vx001
    SPELL_RAPID_BURST       = 63382,
    SPELL_RAPID_BURST_N     = 63387,
    SPELL_RAPID_BURST_N_    = 64019,
    SPELL_RAPID_BURST_H     = 64531,
    SPELL_RAPID_BURST_H_    = 64532,
    SPELL_HAND_PULSE        = 64348,    // only in phase 4
    SPELL_HAND_PULSE_       = 64352,
    SPELL_HAND_PULSE_H      = 64536,
    SPELL_HAND_PULSE_H_     = 64537,
    SPELL_LASER_BARRAGE_DMG = 63293,    // also in phase 4
    SPELL_LASER_BARRAGE     = 63414,
    SPELL_LASER_VISUAL      = 63300,
    SPELL_LASER_TRIGG       = 63274,
    SPELL_ROCKET_STRIKE     = 64064,
    NPC_MIMIRON_FOCUS       = 33835,    //33369
    SPELL_HEAT_WAVE         = 63677,
    SPELL_HEAT_WAVE_H       = 64533,
    SPELL_FLAME_SUPRESS_VX  = 65192,    // used by robot in melee range
    NPC_FOCUSED_LASER       = 34181,

    // frostbomb
    SPELL_FROST_BOMB_EXPL   = 64626,
    SPELL_FROST_BOMB_AURA   = 64624,    // before explode
    //SPELL_FROST_BOMB_VISUAL = 64625,    // bomb grows
    SPELL_FROST_BOMB_SUMMON = 64627,    // summon the frostbomb

    //aerial unit
    SPELL_PLASMA_BALL       = 63689,    // also in phase 4
    SPELL_PLASMA_BALL_H     = 64535,    // also in phase 4
    MOB_ASSAULT_BOT         = 34057,
    MOB_BOMB_BOT            = 33836,
    MOB_BOMB_BOT_321        = 33346,
    MOB_BOMB_BOT_500        = 34192,
    MOB_JUNK_BOT            = 33855,
    SPELL_MAGNETIC_FIELD    = 64668,
    SPELL_MAGNETIC_CORE     = 64436,    // increase dmg taken by 50%, used by magnetic core
    MOB_MAGNETIC_CORE       = 34068,
    ITEM_MAGNETIC_CORE      = 46029,
    SPELL_BOMB_BOT_SUMMON   = 63811,
    SPELL_BOMB_BOT          = 63767,
    SPELL_BOMB_BOT_SACRIFICE= 63801,
    SPELL_GREEN_EFFECT      = 64398,
    SPELL_ORANGE_EFFECT     = 64426,
    SPELL_BLUE_EFFECT       = 64621,

    //hard mode
    // summons fires
    SPELL_EMERGENCY_MODE    = 64582,
    MOB_FROST_BOMB          = 34149,
    MOB_EMERGENCY_FIRE_BOT  = 34147,
    SPELL_DEAFENING_SIREN   = 64616,
    SPELL_WATER_SPRAY       = 64619,

    SPELL_MIMIRONS_INFERNO  = 62910,  // maybe used by rocket
    SPELL_MIMIRONS_INFERNO2 = 62909,  // maybe hard mode
    SPELL_BERSERK           = 26662,

    ACHIEV_FIREFIGHTER      = 3180,
    ACHIEV_FIREFIGHTER_H    = 3189,

    RADIUS                  = 35,     // distance from where trigger for vx barrage is moving
};

enum MimironPhase
{
    PHASE_IDLE      = 0,
    PHASE_INTRO     = 1,
    PHASE_LEVIATHAN = 2,
    PHASE_TRANS_1   = 3,
    PHASE_VX001     = 4,
    PHASE_TRANS_2   = 5,
    PHASE_AERIAL    = 6,
    PHASE_TRANS_3   = 7,
    PHASE_ROBOT     = 8,
    PHASE_OUTRO     = 9,
};

#define CENTER_X            2744.732f
#define CENTER_Y            2569.479f
#define CENTER_Z            364.312f 

const float PosTankHome[2]  = {2794.86f, 2597.83f};
const float PosMimiSafe[2]  = {2784.73f, 2579.31f};
const float PosMimiCenter[2]= {2743.15f, 2561.94f};
const float PosTankCenter[2]= {2745.88f, 2569.25f};

struct LocationsXY
{
    float x, y;
    uint32 id;
};
static LocationsXY SummonLoc[]=
{
    {2753.665f, 2584.712f},
    {2754.150f, 2554.445f},
    {2726.966f, 2569.032f},
    {2759.085f, 2594.249f},
    {2759.977f, 2544.345f},
    {2715.542f, 2569.160f},
    {2765.070f, 2604.337f},
    {2765.676f, 2534.558f},
    {2703.810f, 2569.132f},
};

float const m_fLootMimiron[4] = {2759.0f, 2575.6f, 364.3f, 5.32f};   //mimiron
float const m_fLootMimironH[4] = {2758.9f, 2561.6f, 364.3f, 1.27f};  //mimiron hard

/////////////////////////////
///       VEHICLES        ///
/////////////////////////////
struct MANGOS_DLL_DECL boss_mimiron_vehicleAI : public ScriptedAI
{   
    boss_mimiron_vehicleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance* m_pInstance;

    bool m_bIsRepairing;

    uint32 m_uiDataType;

    void Reset()
    {
        m_bIsRepairing          = false;

        if (m_pInstance) 
            m_pInstance->SetData(m_uiDataType, NOT_STARTED);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetHealth(m_creature->GetMaxHealth());
    }

    void CastFinished(const SpellEntry* spellInfo)
    {
        if (spellInfo->Id == SPELL_SELF_REPAIR)
        {
            m_bIsRepairing = false;
            if (Creature* pMimiron = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(NPC_MIMIRON))))
                AttackStart(pMimiron->getVictim());

            if (m_pInstance)
                m_pInstance->SetData(m_uiDataType, IN_PROGRESS);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        if (Creature* pMimiron = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(NPC_MIMIRON))))
        {
            int32 sayId = 0;
            if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_LEVIATHAN)
                sayId = urand(0,1) ? SAY_TANK_SLAY1 : SAY_TANK_SLAY2;
            else if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_VX001)
                sayId = urand(0,1) ? SAY_TORSO_SLAY1 : SAY_TORSO_SLAY2;
            else if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_AERIAL)
                sayId = urand(0,1) ? SAY_HEAD_SLAY1 : SAY_HEAD_SLAY2;
            else if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_ROBOT)
                sayId = urand(0,1) ? SAY_ROBOT_SLAY1 : SAY_ROBOT_SLAY2;
            if (sayId)
                 DoScriptText(sayId, pMimiron);
        }
    }

    void DamageTaken(Unit *done_by, uint32 &uiDamage)
    {
        if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) >= PHASE_ROBOT)
        {
            if (uiDamage > m_creature->GetHealth())
            {
                uiDamage = 0;
                if (!m_bIsRepairing)
                {
                    m_creature->SetHealth(1);
                    m_creature->InterruptNonMeleeSpells(true);
                    m_creature->RemoveAllAuras();
                    m_creature->GetMotionMaster()->MovementExpired(false);
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_creature->CombatStop();
                    m_bIsRepairing = true;
                    DoCast(m_creature, SPELL_SELF_REPAIR);
                    
                    if (m_pInstance) 
                        m_pInstance->SetData(m_uiDataType, SPECIAL);
                }
            }
        }
    }

    void SuppressFiresInRange(uint32 range)
    {
        std::list<Creature*> lFires;
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34363, range);
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34121, range);
        if (!lFires.empty())
        {
            for(std::list<Creature*>::iterator iter = lFires.begin(); iter != lFires.end(); ++iter)
            {
                if ((*iter) && (*iter)->isAlive())
                    (*iter)->ForcedDespawn();
            }
        }
    }
};
struct MANGOS_DLL_DECL boss_leviathan_mkAI : public boss_mimiron_vehicleAI
{   
    boss_leviathan_mkAI(Creature* pCreature) : boss_mimiron_vehicleAI(pCreature) {  Reset(); }

    bool m_bStartAttack;

    uint32 m_uiMinesTimer;
    uint32 m_uiNapalmTimer;
    uint32 m_uiPlasmaBlastTimer;
    uint32 m_uiShockBlastTimer;

    bool m_bHasSuppresed;
    uint32 m_uiSupressTimer;
    uint32 m_uiSetFireTimer;

    // outro
    bool m_bIsOutro;
    uint32 m_uiOutroTimer;
    uint32 m_uiOutroStep;

    void Reset()
    {
        m_bStartAttack          = false;
        m_uiMinesTimer          = 5000;
        m_uiNapalmTimer         = 20000;
        m_uiPlasmaBlastTimer    = 10000;
        m_uiShockBlastTimer     = 30000;
        m_uiSupressTimer        = 60000;

        m_uiOutroTimer          = 10000;
        m_uiOutroStep           = 1;
        m_bIsOutro              = false;

        m_uiDataType = TYPE_LEVIATHAN_MK;

        boss_mimiron_vehicleAI::Reset();
    }

    void AttackStart(Unit* pWho)
    {
        if (!m_bStartAttack || !pWho)
            return;

        if (m_creature->Attack(pWho, true)) 
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_LEVIATHAN)
                DoStartMovement(pWho);
            else
                SetCombatMovement(false);

            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
    }

    void DamageTaken(Unit *done_by, uint32 &uiDamage)
    {
        if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_LEVIATHAN)
        {
            if (uiDamage > m_creature->GetHealth())
            {
                uiDamage = 0;
                m_bIsOutro = true;
            }
        }
        else
            boss_mimiron_vehicleAI::DamageTaken(done_by, uiDamage);
    }

    void CastFinished(const SpellEntry* spellInfo)
    {
        if (spellInfo->Id == SPELL_FLAME_SUPRESS_MK)
            SuppressFiresInRange(50000);
        else
            boss_mimiron_vehicleAI::CastFinished(spellInfo);
    }

    void SetPhase()
    {
        m_bIsOutro          = false;
        m_uiMinesTimer      = 10000;
        m_uiShockBlastTimer = 30000;
        m_bStartAttack      = true;
        
        // look like a robot
        SetCombatMovement(false);
    }

    void JustDied(Unit* pWho)
    {
        // for debug only
        if (m_pInstance && m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_LEVIATHAN)
            m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_TRANS_1);
    }

    Unit* TargetRandomPreferRanged()
    {
        Unit* target = NULL;
        std::list<Unit*> rangedTargetList;
        ThreatList tList = m_creature->getThreatManager().getPlayerThreatList();
        for (ThreatList::const_iterator itr = tList.begin();itr != tList.end(); ++itr)
        {
            Unit* pUnit = Unit::GetUnit((*m_creature), (*itr)->getUnitGuid());
            if (!pUnit || pUnit->GetDistance(m_creature) < 15.0f)
                continue;

            rangedTargetList.push_back(pUnit);
        }
        uint8 minRanged = 1;
        if (rangedTargetList.size() > minRanged)
        {
            std::list<Unit*>::iterator i = rangedTargetList.begin();
            std::advance(i, rand() % rangedTargetList.size());
            target = *i;
        }
        else
            target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0);

        return target;
    }

    void UpdateAI(const uint32 uiDiff)
    {
       // outro
        if (m_bIsOutro)
        {
            switch(m_uiOutroStep)
            {
                case 1:
                    m_bStartAttack = false;
                    m_creature->RemoveAllAuras();
                    m_creature->DeleteThreatList();
                    m_creature->CombatStop(true);
                    m_creature->InterruptNonMeleeSpells(false);
                    m_creature->SetHealth(m_creature->GetMaxHealth());
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->SendMonsterMoveWithSpeedAndAngle(PosTankHome[0], PosTankHome[1], CENTER_Z, 3.66f, true);
                    
                    if (Creature* pMimiron = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(NPC_MIMIRON))))
                        DoScriptText(SAY_TANK_DEATH, pMimiron);

                    ++m_uiOutroStep;
                    m_uiOutroTimer = 12000;
                    break;
                case 3:
                    if (m_pInstance) 
                        m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_TRANS_1);
                    
                    // reset the miniboss for phase 4
                    m_creature->SetSummonPoint(PosTankHome[0], PosTankHome[1], CENTER_Z, 3.66f);
                    EnterEvadeMode();
                    m_bIsOutro = false;
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 3000;
                    break;
            }

            // Outro Phase Step switcher
            if (m_uiOutroTimer <= uiDiff)
            {
                ++m_uiOutroStep;
                m_uiOutroTimer = 330000;
            } m_uiOutroTimer -= uiDiff;
            return;
        }
        
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_bIsRepairing)
            return;
        
        // only Leviathan Phase
        if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_LEVIATHAN)
        {
            // Plasma Blast
            if (HandleTimer(m_uiPlasmaBlastTimer, uiDiff))
            {
                DoScriptText(EMOTE_PLASMA_BLAST, m_creature);
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_PLASMA_BLAST : SPELL_PLASMA_BLAST_H);
                m_uiPlasmaBlastTimer = 30000;
                m_uiShockBlastTimer = 9000;
            }

            // Napalm Shell
            if (HandleTimer(m_uiNapalmTimer,uiDiff))
            {
                if (Unit* pTarget = SelectRandomPlayerPreferRanged(1, 15))
                {
                    DoCast(pTarget, m_bIsRegularMode ? SPELL_NAPALM_SHELL : SPELL_NAPALM_SHELL_H);
                    m_uiNapalmTimer = 6000;
                }
            }

            // hard mode script
            if (m_pInstance->GetData(TYPE_MIMIRON_HARD) == IN_PROGRESS)
            {
                // Flame Suppression
                if (HandleTimer(m_uiSupressTimer, uiDiff))
                {
                    DoCast(m_creature, SPELL_FLAME_SUPRESS_MK);
                    m_uiSupressTimer = 60000;
                }
            }
        }

        // proximity mines
        if (HandleTimer(m_uiMinesTimer, uiDiff))
        {
            for(uint8 i = 0; i < urand(8, 10); ++i)
            {
                float angle = urand(0, 2*M_PI_F);
                float homeX = m_creature->GetPositionX() + 15*cos(angle);
                float homeY = m_creature->GetPositionY() + 15*sin(angle);
                m_creature->SummonCreature(MOB_PROXIMITY_MINE, homeX, homeY, m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 10000);
            }
            m_uiMinesTimer = 35000;
        }
        
        // shock blast
        if (HandleTimer(m_uiShockBlastTimer, uiDiff))
        {
            DoCast(m_creature, SPELL_SHOCK_BLAST);
            m_uiShockBlastTimer = 50000;
        }
        
        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_vx001AI : public boss_mimiron_vehicleAI
{   
    boss_vx001AI(Creature* pCreature) : boss_mimiron_vehicleAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }

    bool m_bStartAttack;

    uint32 m_uiRapidBurstTimer;
    uint32 m_uiLaserBarrageTimer;
    uint32 m_uiRocketStrikeTimer;
    uint32 m_uiHeatWaveTimer;
    uint32 m_uiHandPulseTimer;

    uint32 m_uiFlameSuppressTimer;
    uint32 m_uiFrostBombTimer;

    Creature* pLaser;
    uint64 m_uiFocusedLaserGUID;

    uint16 m_uiHandPulseCount;

    bool m_bIsCastingBarrage;

    void Reset()
    {
        m_bStartAttack          = false;

        m_uiRapidBurstTimer     = 1000;
        m_uiLaserBarrageTimer   = 41000;
        m_uiRocketStrikeTimer   = 25000;
        m_uiHeatWaveTimer       = 20000;
        m_uiHandPulseTimer      = 1000;
        m_uiHandPulseCount      = 0;

        m_uiFlameSuppressTimer  = urand(7000, 13000);
        m_uiFrostBombTimer      = 30000;

        m_uiFocusedLaserGUID    = 0;

        m_bIsCastingBarrage     = false;
        pLaser                  = NULL;

        SetCombatMovement(false);

        m_uiDataType = TYPE_VX001;

        boss_mimiron_vehicleAI::Reset();
    }

    void AttackStart(Unit* pWho)
    {
        if (!m_bStartAttack)
            return;

        if (m_creature->Attack(pWho, true)) 
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            m_creature->SetInCombatWithZone();
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
    }

    void SetPhase()
    {
        m_uiLaserBarrageTimer   = 60000;
        m_uiRocketStrikeTimer   = 25000;
        m_uiHandPulseTimer      = 5000;

        m_bStartAttack          = true;

        // look like a robot
        m_creature->GetMotionMaster()->MoveIdle();
       
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_VX001)
            {
                if (Creature* pMimiron = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(NPC_MIMIRON))))
                    DoScriptText(SAY_TORSO_DEATH, pMimiron);
                m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_TRANS_2);
                m_pInstance->SetData(TYPE_VX001, DONE);
            }
        }
    }

    Creature* SelectRandomFire()
    {
        std::list<Creature*> lFires;
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34363, DEFAULT_VISIBILITY_INSTANCE);
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34121, DEFAULT_VISIBILITY_INSTANCE);

        if (!lFires.empty())
        {
            std::list<Creature* >::iterator iter = lFires.begin();
            advance(iter, urand(0, lFires.size()-1));

            if ((*iter)->isAlive())
                return *iter;
        }

        return 0;
    }

    void CastFinished(const SpellEntry* spellInfo)
    {
        // Laser Barrage
        if (spellInfo->Id == SPELL_LASER_BARRAGE)
        {
            if (!pLaser)
                pLaser = (Creature*)Unit::GetUnit(*m_creature, m_uiFocusedLaserGUID);

            if (!pLaser)
                return;

            m_creature->CastSpell(pLaser, SPELL_LASER_TRIGG, false);
            
            float o = m_creature->GetAngle(pLaser);
            float max_o = 2*M_PI_F;
            o -= max_o/3;
            
            // normalize o
            if (o < 0)
                o += max_o;

            float x = m_creature->GetPositionX() + RADIUS*cos(o);
            float y = m_creature->GetPositionY() + RADIUS*sin(o);
            float z = m_creature->GetPositionZ();
            m_bIsCastingBarrage = true;
            
            pLaser->SetSpeedRate(MOVE_RUN, 0.8f);
            pLaser->GetMotionMaster()->MovePoint(0, x, y, z, false);
        }

        // Frost Bomb
        else if (spellInfo->Id == 64623)
        {
            if (Creature* crt = SelectRandomFire())
                m_creature->SummonCreature(34149, crt->GetPositionX(), crt->GetPositionY(), crt->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 20000);
        }
        // Flame Suppressant
        else if (spellInfo->Id == SPELL_FLAME_SUPRESS_VX)
            SuppressFiresInRange(10);
        else
            boss_mimiron_vehicleAI::CastFinished(spellInfo);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // setting orientantation while casting barrage
        if (m_bIsCastingBarrage)
        {
            if (m_creature->IsNonMeleeSpellCasted(false))
            {
                if (!pLaser && m_uiFocusedLaserGUID)
                    pLaser = (Creature*)Unit::GetUnit(*m_creature, m_uiFocusedLaserGUID);
                m_creature->FixOrientation(m_creature->GetAngle(pLaser));
            }
            else
            {
                m_uiFocusedLaserGUID = 0;
                m_bIsCastingBarrage = false;
                m_creature->FixOrientation();
            }
        }
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_bIsRepairing)
            return;

        // hard mode
        if (m_pInstance->GetData(TYPE_MIMIRON_HARD) == IN_PROGRESS)
        {
            // Frost Bomb
            if (HandleTimer(m_uiFrostBombTimer,uiDiff))
            {
                DoCast(m_creature, 64623, false);
                m_uiFrostBombTimer = 30000;
            }

            // only in VX001 phase
            if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_VX001)
            {
                // Flame Suppress
                if (HandleTimer(m_uiFlameSuppressTimer, uiDiff))
                {
                    DoCast(m_creature, SPELL_FLAME_SUPRESS_VX);
                    m_uiFlameSuppressTimer = urand(7000, 13000);
                }
            }
        }

        // only in robot phase
        if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_ROBOT)
        {
            // Hand Pulse
            if (HandleTimer(m_uiHandPulseTimer, uiDiff, true))
            {
                if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                {
                    uint32 spellId = 0;
                    bool leftOrRight = ++m_uiHandPulseCount % 2;
                    if (m_bIsRegularMode)
                        spellId = leftOrRight ? SPELL_HAND_PULSE : SPELL_HAND_PULSE_;
                    else
                        spellId = leftOrRight ? SPELL_HAND_PULSE_H : SPELL_HAND_PULSE_H_;

                    DoCast(pTarget, spellId);
                }
                m_uiHandPulseTimer = 1000;
            }else m_uiHandPulseTimer -= uiDiff;
        }

        // Laser Barrage
        if (HandleTimer(m_uiLaserBarrageTimer, uiDiff, true))
        {
            float o = m_creature->GetOrientation();
            float x = m_creature->GetPositionX() + RADIUS*cos(o);
            float y = m_creature->GetPositionY() + RADIUS*sin(o);
            if (pLaser = m_creature->SummonCreature(NPC_FOCUSED_LASER, x, y, m_creature->GetPositionZ(), o, TEMPSUMMON_TIMED_DESPAWN, 16000))
            {
                m_creature->CastSpell(pLaser, SPELL_LASER_BARRAGE, false);
                m_uiFocusedLaserGUID = pLaser->GetGUID();
                m_uiLaserBarrageTimer = 55000;
            }
        }

        // Rocket Strike
        if (HandleTimer(m_uiRocketStrikeTimer, uiDiff, true))
        {
            if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
            {
                if (Creature* pTemp = m_creature->SummonCreature(NPC_MIMIRON_FOCUS, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 30000))
                {
                    pTemp->setFaction(14);
                    pTemp->GetMotionMaster()->MoveIdle();
                    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pTemp->CombatStop();
                    pTemp->SetDisplayId(11686);     // make invisible
                    pTemp->CastSpell(pTemp, SPELL_ROCKET_STRIKE, false);
                }
            }
            m_uiRocketStrikeTimer = urand(25000, 30000);
        }

        // only in VX001 phase
        if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_VX001)
        {
            // Heat Wave
            if (HandleTimer(m_uiHeatWaveTimer, uiDiff))
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_HEAT_WAVE : SPELL_HEAT_WAVE_H);
                m_uiHeatWaveTimer = 10000;
            }

            // Rapid Burst
            if (HandleTimer(m_uiRapidBurstTimer, uiDiff))
            {
                if (Unit* pTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                {
                    m_creature->SetFacingToObject(pTarget);
                    DoCast(pTarget, SPELL_RAPID_BURST);
                }
                m_uiRapidBurstTimer = 4000;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_aerial_command_unitAI : public boss_mimiron_vehicleAI
{   
    boss_aerial_command_unitAI(Creature* pCreature) : boss_mimiron_vehicleAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }

    bool m_bStartAttack;
    
    uint32 m_uiPlasmaBallTimer;
    uint32 m_uiSpreadFiresTimer;

    uint32 m_uiJunkBotTimer;
    uint32 m_uiAssaultBotTimer;
    uint32 m_uiBombBotTimer;
    uint32 m_uiFireBotTimer;

    void Reset()
    {
        m_bStartAttack          = false;
        m_uiSpreadFiresTimer    = urand(40000, 50000);

        m_uiPlasmaBallTimer     = 3000;

        m_uiJunkBotTimer        = urand(6000, 8000);
        m_uiAssaultBotTimer     = urand(1000, 3000);
        m_uiBombBotTimer        = urand(8000, 10000);
        m_uiFireBotTimer        = 30000;

        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
        m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
        SetCombatMovement(false);
        
        m_uiDataType = TYPE_AERIAL_UNIT;
        
        boss_mimiron_vehicleAI::Reset();
    }

    void AttackStart(Unit* pWho)
    {
        if (!m_bStartAttack)
            return;

        if (m_creature->Attack(pWho, true)) 
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            m_creature->SetInCombatWithZone();
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
    }

    void JustDied(Unit *killer)
    {
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_AERIAL)
            {
                if (Creature* pMimiron = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(NPC_MIMIRON))))
                    DoScriptText(SAY_HEAD_DEATH, pMimiron);
                m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_TRANS_3);
                m_pInstance->SetData(TYPE_AERIAL_UNIT, DONE);
            }
        }
    }

    void SetPhase()
    {
        m_uiPlasmaBallTimer = 1000;
        m_bStartAttack      = true;

        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 0);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
        m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);

        SetCombatMovement(false);
    }
    
    void JustSummoned(Creature* pSummon)
    {
        pSummon->SetInCombatWithZone();
        if (pSummon->GetEntry() == MOB_EMERGENCY_FIRE_BOT){}
        else if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
            m_creature->AI()->AttackStart(plr);
    }

    Creature* SelectRandomSummonPoint()
    {
        std::list<Creature*> m_lSummon;
        GetCreatureListWithEntryInGrid(m_lSummon, m_creature, 33856, DEFAULT_VISIBILITY_INSTANCE);

        std::list<Creature*>::iterator itr;
        for(itr = m_lSummon.begin(); itr != m_lSummon.end(); ++itr)
        {
            if ((*itr)->HasAuraType(SPELL_AURA_DUMMY))
            {
                m_lSummon.erase(itr);
                itr = m_lSummon.begin();
            }
        }
        
        if (m_lSummon.empty())
            return 0;

        itr = m_lSummon.begin();
        std::advance(itr, urand(0, m_lSummon.size()-1));

        return *itr;
    }

    std::list<Creature*> GetRandomSummonPointsPreferLine()
    {
        std::list<Creature*> m_lSummon;
        GetCreatureListWithEntryInGrid(m_lSummon, m_creature, 33856, DEFAULT_VISIBILITY_INSTANCE);

        // split creatures into list by lines
        std::list< std::list<Creature*> > m_lLists;
        for (uint8 i = 1; i < 4; ++i)
        {
            std::list<Creature*> m_lLine;
            for(std::list<Creature*>::iterator itr = m_lSummon.begin(); itr != m_lSummon.end(); ++itr)
                if (isCorrectPoint(m_creature->GetDistance(*itr), i))
                    m_lLine.push_back(*itr);

            m_lLists.push_back(m_lLine);
        }

        // remove lines where one creature is already casting
        for(std::list< std::list<Creature*> >::iterator iter = m_lLists.begin(); iter != m_lLists.end(); ++iter)
        {
            for(std::list<Creature*>::iterator itr = (*iter).begin(); itr != (*iter).end(); ++itr)
            {
                if ((*itr)->HasAuraType(SPELL_AURA_DUMMY))
                {
                    m_lLists.erase(iter);
                    iter = m_lLists.begin();
                    break;
                }
            }
        }

        // select random line from remaing
        if (!m_lLists.empty())
        {
            std::list< std::list<Creature*> >::iterator itr = m_lLists.begin();
            std::advance(itr, urand(0, m_lLists.size()-1));
            return *itr;
        }
        // if no line remains select random targets in maximum of three
        else
        {
            std::list<Creature*> m_lListOfLastHope;
            for (uint8 i = 0; i < 3; ++i)
            {
                std::list<Creature*>::iterator itr = m_lSummon.begin();
                for (uint8 y = 0; y !=  m_lSummon.size()-1; ++y)
                {
                    std::advance(itr, y);
                    if (!(*itr)->HasAuraType(SPELL_AURA_DUMMY))
                    {
                        m_lListOfLastHope.push_back(*itr);
                        break;
                    }
                }
            }
            return m_lListOfLastHope;
        }
    }

    bool isCorrectPoint(uint32 distance, uint8 line)
    {
        return line < 3 && ProperLineForDistance(distance) == line;
    }
    uint32 ProperLineForDistance(uint32 distance)
    {
        if (distance > 35)
            return 3;
        else if (distance > 25)
            return 2;

        return 1;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_bIsRepairing)
            return;

        // only phase 3, this happens event while boss has Magnetic Core
        if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_AERIAL)
        {
            // Junk Bot
            if (HandleTimer(m_uiJunkBotTimer, uiDiff, true))
            {
                if (Creature* pSummon = SelectRandomSummonPoint())
                    pSummon->CastSpell(pSummon, 64398, true);
                m_uiJunkBotTimer = urand(10000, 15000);
            }

            // Assault Bot
            if (HandleTimer(m_uiAssaultBotTimer, uiDiff, true))
            {
                if (Creature* pSummon = SelectRandomSummonPoint())
                    pSummon->CastSpell(pSummon, 64426, true);
                m_uiAssaultBotTimer = urand(20000, 30000);
            }

            // hard mode
            if (m_pInstance->GetData(TYPE_MIMIRON_HARD) == IN_PROGRESS)
            {
                // Emergency fire bot
                if (HandleTimer(m_uiFireBotTimer, uiDiff, true))
                {
                    std::list<Creature*> m_lPoints = GetRandomSummonPointsPreferLine();
                    for (std::list<Creature*>::iterator itr = m_lPoints.begin(); itr != m_lPoints.end(); ++itr)
                        if (Creature* pPoint = *itr)
                            pPoint->CastSpell(pPoint, 64621, true);

                    m_uiFireBotTimer = 30000;
                }
            }
        }

        // if has magnetic core stop UpdateAI
        if (m_creature->HasAura(SPELL_MAGNETIC_CORE))
            return;

        // spawn adds in arena, only in phase 3
        if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_AERIAL)
        {
            // Bomb bot
            if (HandleTimer(m_uiBombBotTimer, uiDiff))
            {
                m_creature->CastSpell(m_creature, SPELL_BOMB_BOT_SUMMON, false);
                m_uiBombBotTimer = urand(10000, 15000);
            }
        }
 
        // Plasma Ball
        if (HandleTimer(m_uiPlasmaBallTimer, uiDiff))
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_PLASMA_BALL : SPELL_PLASMA_BALL_H);
            m_uiPlasmaBallTimer = m_bIsRegularMode ? 2500 : 2000;
        }
    }
};


/////////////////////////////
///       MIMIRON         ///
/////////////////////////////
struct MANGOS_DLL_DECL boss_mimironAI : public ScriptedAI
{   
    boss_mimironAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        pTank = NULL;
        SetCombatMovement(false);
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance *m_pInstance;

    bool m_bIsHardMode;
    uint32 m_uiSelfDestructTimer;
    uint32 m_uiSelfRepairTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiOutroTimer;
    uint32 m_uiSpreadTimer;

    uint32 m_uiTimer;
    uint32 m_uiPhaseStep;
    uint32 m_uiPhase;
 
    uint64 m_uiTankGUID;
    uint64 m_uiTorsoGUID;
    uint64 m_uiHeadGUID;

    bool m_bCatchPhraseSaid;

    Creature* pTank;

    void Reset()
    {
        m_bIsHardMode           = false;
        m_uiSelfDestructTimer   = 460000;  // 8 min
        m_uiPhase               = PHASE_IDLE;
        m_uiBerserkTimer        = 900000;   // 15 min

        m_uiTimer               = 1;
        m_uiPhaseStep           = 1;

        m_uiTankGUID            = 0;
        m_uiTorsoGUID           = 0;
        m_uiHeadGUID            = 0;

        m_uiSpreadTimer         = 25000;

        m_bCatchPhraseSaid      = false;

        if (GameObject* pButton = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_MIMIRON_BUTTON)))
            pButton->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);

        if (GameObject* pLift = GetClosestGameObjectWithEntry(m_creature, GO_MIMIRON_ELEVATOR, DEFAULT_VISIBILITY_INSTANCE))
            pLift->SetGoState(GO_STATE_ACTIVE);

        if (Creature* pTorso = GetClosestCreatureWithEntry(m_creature, NPC_VX001, 80.0f))
            pTorso->DealDamage(pTorso, pTorso->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

        if (Creature* pHead = GetClosestCreatureWithEntry(m_creature, NPC_AERIAL_UNIT, 80.0f))
            pHead->DealDamage(pHead, pHead->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_IDLE);
            m_pInstance->SetData(TYPE_MIMIRON_HARD, NOT_STARTED);
        }

        // try to find tank or summon new one
        if (pTank = GetTank() ? GetTank() : m_creature->SummonCreature(NPC_LEVIATHAN_MK, PosTankCenter[0], PosTankCenter[1], CENTER_Z, M_PI_F, TEMPSUMMON_MANUAL_DESPAWN, 0))
        {
            m_uiTankGUID = pTank->GetGUID();
            if (pTank->isAlive())
            {
                pTank->SetSummonPoint(PosTankCenter[0], PosTankCenter[1], CENTER_Z, M_PI_F);
                pTank->AI()->EnterEvadeMode();
                pTank->SetHealth(pTank->GetMaxHealth());
            }
            else
                pTank->Respawn();
        }

        m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
        
        m_creature->HandleEmoteCommand(EMOTE_STATE_WORK);
        m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_WORK);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        
    }

    void JustReachedHome()
    {
        if (m_pInstance) 
            m_pInstance->SetData(TYPE_MIMIRON, FAIL);
    }

    Creature* GetTank()
    {
        if (pTank)
            return pTank;

        if (m_uiTankGUID)
            if (Creature* pTemp = m_pInstance->instance->GetCreature(m_uiTankGUID))
                return pTemp;

        return GetClosestCreatureWithEntry(m_creature, NPC_LEVIATHAN_MK, 80.0f);

    }
    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER && pWho->isTargetableForAttack())
        {
            if (!m_bCatchPhraseSaid && m_creature->GetDistance(pWho) < 65)
            {
                DoScriptText(SAY_AGGRO, m_creature);
                m_bCatchPhraseSaid = true;
            }
            if (!m_creature->isInCombat() && m_creature->GetDistance(pWho) < 25)
                AttackStart(pWho);
        }
    }
    void Aggro(Unit *who) 
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_MIMIRON, IN_PROGRESS);
        m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_INTRO);
        m_uiPhase = PHASE_INTRO;
        m_pInstance->SetData(TYPE_MIMIRON_HARD, NOT_STARTED);
        
        if (m_bIsHardMode)
        {
            m_pInstance->SetData(TYPE_MIMIRON_HARD, IN_PROGRESS);
            SpreadFires();
        }

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (m_pInstance->GetData(TYPE_MIMIRON_TP) != DONE)
            m_pInstance->SetData(TYPE_MIMIRON_TP, DONE);
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            m_creature->SetInCombatWithZone();
        }
    }

    void DoOutro()
    {
        if (m_pInstance) 
        {
            m_pInstance->SetData(TYPE_MIMIRON, DONE);
            // spawn loot chest
            m_creature->SummonGameobject(m_bIsRegularMode ? LOOT_MIMIRON : LOOT_MIMIRON_H, 
                m_fLootMimiron[0], m_fLootMimiron[1], m_fLootMimiron[2], m_fLootMimiron[3], 604800);
            if (m_bIsHardMode)
            {
                m_pInstance->SetData(TYPE_MIMIRON_HARD, DONE);
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_FIREFIGHTER : ACHIEV_FIREFIGHTER_H);
                // spawn hard loot chest
                m_creature->SummonGameobject(m_bIsRegularMode ? LOOT_MIMIRON_HARD : LOOT_MIMIRON_HARD_H, 
                    m_fLootMimironH[0], m_fLootMimironH[1], m_fLootMimironH[2], m_fLootMimironH[3], 604800);
            }
        }

        m_creature->SetHardModeKill(m_bIsHardMode);
        if (Unit* victim = m_creature->getVictim())
            if (victim->GetTypeId() == TYPEID_PLAYER)
                m_creature->LogKill((Player*)victim);

        m_creature->ForcedDespawn();
    }

    // for debug only
    void JustDied(Unit* pKiller)
    {
       if (m_pInstance) 
        {
            m_pInstance->SetData(TYPE_MIMIRON, DONE);
            if (m_bIsHardMode)
                m_pInstance->SetData(TYPE_MIMIRON_HARD, DONE);
        }
    }

    void SpreadFires()
    {
        if (!m_pInstance)
            return;

        Map::PlayerList const& lPlayers = m_pInstance->instance->GetPlayers();
        std::list<Player*> m_lPlayers;

        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            if (Player* plr = itr->getSource())
                if (plr->isAlive() && plr->GetDistance2d(CENTER_X, CENTER_Y) < 100)
                    m_lPlayers.push_back(plr);

        uint8 m_uiTargetsCount = m_lPlayers.size() < 3 ? m_lPlayers.size() : 3;
        for(uint8 i = 0; i < m_uiTargetsCount; ++i)
        {
            std::list<Player*>::iterator itr = m_lPlayers.begin();
            std::advance(itr, urand(0, m_lPlayers.size()-1));
            if (Player* plr = *itr)
                m_creature->CastSpell(plr, SPELL_FLAMES_SUMMON, true);

            m_lPlayers.erase(itr);
        }

        m_uiSpreadTimer = m_uiPhase == PHASE_ROBOT ? 17000 : 22000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPhase == m_pInstance->GetData(TYPE_MIMIRON_PHASE))
        {
            switch(m_uiPhase)
            {
                case PHASE_INTRO:
                {
                    switch(m_uiPhaseStep)
                    {
                        case 1:
                        {
                            ++m_uiPhaseStep;
                            m_uiTimer = 5000;
                            break;
                        }
                        case 3:
                        {
                            // cancel emote, aggro say, disable red button, find tank
                            m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                            DoScriptText(m_bIsHardMode ? SAY_HARD_MODE : SAY_TANK_ACTIVE, m_creature);

                            if (GameObject* pButton = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_MIMIRON_BUTTON)))
                                pButton->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
                            ++m_uiPhaseStep;
                            m_uiTimer = 15000;
                            break;
                        }
                        case 5:
                        {
                            // activate tank, start encounter
                            if (pTank = GetTank())
                            {
                                if (m_pInstance)
                                    m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_LEVIATHAN);
                                   
                                ((boss_leviathan_mkAI*)pTank->AI())->m_bStartAttack = true;
                                pTank->AI()->AttackStart(m_creature->getVictim());
                                m_uiBerserkTimer = 900000;   // 15 min

                                m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                                m_creature->SendMonsterMoveWithSpeedAndAngle(PosMimiSafe[0], PosMimiSafe[1], CENTER_Z, M_PI_F, true);

                                if (m_bIsHardMode)
                                {
                                    m_uiSelfDestructTimer = 460000;  // 8 min
                                    pTank->CastSpell(pTank, SPELL_EMERGENCY_MODE, true);
                                    pTank->SetHealth(pTank->GetMaxHealth());
                                }
                                ++m_uiPhase;
                                m_uiPhaseStep = 1;
                                m_uiTimer = 10000;
                            }
                            break;
                        }
                    }
                    break;
                }
                case PHASE_LEVIATHAN:
                    // leviathan MK phase: see above script
                    ++m_uiPhase;
                    break;
                case PHASE_TRANS_1:
                {
                    switch(m_uiPhaseStep)
                    {
                        case 1:
                        {
                            ++m_uiPhaseStep;
                            m_uiTimer = 5000;
                            break;
                        }
                        case 3:
                        {
                            // lift up elevator
                            if (GameObject* pLift = GetClosestGameObjectWithEntry(m_creature, GO_MIMIRON_ELEVATOR, DEFAULT_VISIBILITY_INSTANCE))
                                m_pInstance->DoUseDoorOrButton(pLift->GetGUID());
                            ++m_uiPhaseStep;
                            m_uiTimer = 14000; 
                            break;
                        }
                        case 5:
                        {
                            // summon boss
                            if (Creature* pTorso = m_creature->SummonCreature(NPC_VX001, CENTER_X, CENTER_Y, CENTER_Z, M_PI_F, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                                m_uiTorsoGUID = pTorso->GetGUID();
                            // close elevator
                            if (GameObject* pLift = GetClosestGameObjectWithEntry(m_creature, GO_MIMIRON_ELEVATOR, DEFAULT_VISIBILITY_INSTANCE))
                                pLift->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);

                            m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                            uint32 timeToCenter = m_creature->SendMonsterMoveWithSpeedAndAngle(PosMimiCenter[0], PosMimiCenter[1], CENTER_Z, M_PI_F/2, true);
                            
                            ++m_uiPhaseStep;
                            m_uiTimer = timeToCenter+2000;
                            break;
                        }
                        case 7:
                        {
                            // say activate torso
                            DoScriptText(SAY_TORSO_ACTIVE, m_creature);
                            ++m_uiPhaseStep;
                            m_uiTimer = 8000;
                            break;
                        }
                        case 9:
                        {
                            // activate VX-001
                            if (Creature* pTorso = m_pInstance->instance->GetCreature(m_uiTorsoGUID))
                            {
                                m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_VX001);
                                
                                ((boss_vx001AI*)pTorso->AI())->m_bStartAttack = true;
                                pTorso->AI()->AttackStart(m_creature->getVictim());

                                m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                                m_creature->SendMonsterMoveWithSpeedAndAngle(PosMimiSafe[0], PosMimiSafe[1], CENTER_Z, M_PI_F, true);

                                if (m_bIsHardMode)
                                {
                                    pTorso->CastSpell(pTorso, SPELL_EMERGENCY_MODE, true);
                                    pTorso->SetHealth(pTorso->GetMaxHealth());
                                }
                            }
                            ++m_uiPhase;
                            m_uiPhaseStep = 1;
                            m_uiTimer = 10000;
                            break;
                        }
                    }
                    break;
                }
                case PHASE_VX001:
                    // VX001 phase: see above script
                    ++m_uiPhase;
                    break;
                case PHASE_TRANS_2:
                {
                    switch(m_uiPhaseStep)
                    {
                        case 1:
                        {
                            ++m_uiPhaseStep;
                            m_uiTimer = 5000;
                            break;
                        }
                        case 3:
                        {
                            // move mimiron to center
                            m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                            uint32 timeToCenter = m_creature->SendMonsterMoveWithSpeedAndAngle(PosMimiCenter[0], PosMimiCenter[1], CENTER_Z, M_PI_F/2, true);
                            
                            ++m_uiPhaseStep;
                            m_uiTimer = timeToCenter+2000;
                            break;
                        }
                        case 5:
                        {
                            // do say and summon boss
                            DoScriptText(SAY_HEAD_ACTIVE, m_creature);
                            if (Creature* pHead = m_creature->SummonCreature(NPC_AERIAL_UNIT, CENTER_X, CENTER_Y, CENTER_Z+9, M_PI_F, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                                m_uiHeadGUID = pHead->GetGUID();

                            ++m_uiPhaseStep;                            
                            m_uiTimer = 10000;
                            break;
                        }
                        case 7:
                        {
                            // active boss and send mimiron to safe
                            if (Creature* pHead = m_pInstance->instance->GetCreature(m_uiHeadGUID))
                            {
                                m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_AERIAL);

                                ((boss_aerial_command_unitAI*)pHead->AI())->m_bStartAttack = true;
                                pHead->AI()->AttackStart(m_creature->getVictim());

                                m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                                m_creature->SendMonsterMoveWithSpeedAndAngle(PosMimiSafe[0], PosMimiSafe[1], CENTER_Z, M_PI_F, true);

                                if (m_bIsHardMode)
                                {
                                    pHead->CastSpell(pHead, SPELL_EMERGENCY_MODE, true);
                                    pHead->SetHealth(pHead->GetMaxHealth());
                                }
                            }
                            ++m_uiPhase;
                            m_uiPhaseStep = 1;
                            m_uiTimer = 10000;
                        }
                    }
                    break;
                }
                case PHASE_AERIAL:
                    // Aerial Unit phase: see above script
                    ++m_uiPhase;
                    break;
                case PHASE_TRANS_3:
                {
                    switch(m_uiPhaseStep)
                    {
                        case 1:
                        {
                            ++m_uiPhaseStep;
                            m_uiTimer = 5000;
                            break;
                        }
                        case 3:
                        {
                            // move tank to center
                            if (pTank = GetTank())
                            {
                                uint32 timeToCenter = pTank->SendMonsterMoveWithSpeedAndAngle(CENTER_X, CENTER_Y, CENTER_Z, M_PI_F/2, true);

                                ++m_uiPhaseStep;
                                m_uiTimer = timeToCenter+2000;
                            }
                            break;
                        }
                        case 5:
                        {
                            // spawn torso and head
                            if (Creature* pTorso = m_creature->SummonCreature(NPC_VX001, CENTER_X, CENTER_Y, CENTER_Z+4, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000))
                                m_uiTorsoGUID = pTorso->GetGUID();
                            
                            if (Creature* pHead = m_creature->SummonCreature(NPC_AERIAL_UNIT, CENTER_X, CENTER_Y, CENTER_Z+12, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000))
                                m_uiHeadGUID = pHead->GetGUID();

                            ++m_uiPhaseStep;
                            m_uiTimer = 5000;
                            break;
                        }
                        case 7:
                        {
                            // activate them all
                            DoScriptText(SAY_ROBOT_ACTIVE, m_creature);

                            if (pTank = GetTank())
                            {
                                ((boss_leviathan_mkAI*)pTank->AI())->SetPhase();
                                pTank->AI()->AttackStart(m_creature->getVictim());

                                if (m_bIsHardMode)
                                    pTank->CastSpell(pTank, SPELL_EMERGENCY_MODE, false);

                                pTank->SetHealth(pTank->GetMaxHealth()/2);
                            }

                            if (Creature* pTorso = m_pInstance->instance->GetCreature(m_uiTorsoGUID))
                            {
                                ((boss_vx001AI*)pTorso->AI())->SetPhase();
                                pTorso->AI()->AttackStart(m_creature->getVictim());

                                if (m_bIsHardMode)
                                    pTorso->CastSpell(pTorso, SPELL_EMERGENCY_MODE, false);

                                pTorso->SetHealth(pTorso->GetMaxHealth()/2);
                            }

                            if (Creature* pHead = m_pInstance->instance->GetCreature(m_uiHeadGUID))
                            {
                                ((boss_aerial_command_unitAI*)pHead->AI())->SetPhase();
                                pHead->AI()->AttackStart(m_creature->getVictim());

                                if (m_bIsHardMode)
                                    pHead->CastSpell(pHead, SPELL_EMERGENCY_MODE, false);

                                pHead->SetHealth(pHead->GetMaxHealth()/2);
                            }

                            m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_ROBOT);

                            ++m_uiPhase;
                            m_uiPhaseStep = 1;
                            m_uiTimer = 10000;
                        }
                    }
                    break;
                }
                case PHASE_ROBOT:
                {
                    if (m_uiPhaseStep == 1)
                    {
                        // if all parts have died
                        if (m_pInstance->GetData(TYPE_LEVIATHAN_MK) == SPECIAL &&
                           m_pInstance->GetData(TYPE_VX001) == SPECIAL &&
                           m_pInstance->GetData(TYPE_AERIAL_UNIT) == SPECIAL)
                        {
                            if (pTank = GetTank())
                                pTank->InterruptNonMeleeSpells(false);
                            
                            if (Creature* pTorso = m_pInstance->instance->GetCreature(m_uiTorsoGUID))
                                pTorso->InterruptNonMeleeSpells(false);

                            if (Creature* pHead = m_pInstance->instance->GetCreature(m_uiHeadGUID))
                                pHead->InterruptNonMeleeSpells(false);

                            m_pInstance->SetData(TYPE_LEVIATHAN_MK, DONE);
                            m_pInstance->SetData(TYPE_VX001, DONE);
                            m_pInstance->SetData(TYPE_AERIAL_UNIT, DONE);

                            DoScriptText(SAY_ROBOT_DEATH, m_creature);
                            m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_OUTRO);

                            m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                            uint32 timeToCenter = m_creature->SendMonsterMoveWithSpeedAndAngle(CENTER_X, CENTER_Y, CENTER_Z, M_PI_F, true);

                            ++m_uiPhase;
                            m_uiPhaseStep = 0;
                            m_uiTimer = timeToCenter+5000;
                        }
                        else
                        {
                            // lower step to zero and after timer runs up step will be again 1
                            m_uiTimer = 2000;
                            --m_uiPhaseStep;
                        }

                    }
                    break;
                }
                case PHASE_OUTRO:
                {
                    if (m_uiPhaseStep == 1)
                    {
                        if (pTank = m_pInstance->instance->GetCreature(m_uiTankGUID))
                            m_creature->DealDamage(pTank, pTank->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        if (Creature* pHead = m_pInstance->instance->GetCreature(m_uiHeadGUID))
                            m_creature->DealDamage(pHead, pHead->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        if (Creature* pTorso = m_pInstance->instance->GetCreature(m_uiTorsoGUID))
                            m_creature->DealDamage(pTorso, pTorso->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        DoOutro();
                        m_uiPhaseStep = 0;
                    }
                    break;
                }
            }
        
            // if instance phase is in same phase as boss phase ten check intro
            if (m_uiTimer <= uiDiff)
            {
                ++m_uiPhaseStep;
                m_uiTimer = 330000;
            }else m_uiTimer -= uiDiff;
        }

        // after robot phase do not berserk, selfdestruct, etc.
        if (m_uiPhase > PHASE_ROBOT)
            return;

        if (m_bIsHardMode)
        {
            // self destruct platform in hard mode
            if (HandleTimer(m_uiSelfDestructTimer, uiDiff, true))
            {
                m_creature->SummonCreature(NPC_MIMIRON_INFERNO, CENTER_X, CENTER_Y, CENTER_Z, 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
                // visual part
                if (Creature* pTemp = m_creature->SummonCreature(NPC_MIMIRON_FOCUS, CENTER_X, CENTER_Y, CENTER_Z, 0, TEMPSUMMON_TIMED_DESPAWN, 15000))
                {
                    pTemp->GetMotionMaster()->MoveIdle();
                    pTemp->CombatStop();
                    pTemp->SetDisplayId(11686);     // make invisible
                    pTemp->CastSpell(pTemp, SPELL_SELF_DESTRUCTION, false);
                }
                m_uiSelfDestructTimer = 10000;
            }

            // spread fire to new 3 players only in hard mode
            if (HandleTimer(m_uiSpreadTimer, uiDiff, true))
                SpreadFires();
        }


        // berserk
        if (HandleTimer(m_uiBerserkTimer, uiDiff))
        {
            if (pTank = GetTank())
                if (pTank && pTank->isAlive())
                    pTank->CastSpell(pTank, SPELL_BERSERK, false);
            
            if (Creature* pTorso = m_pInstance->instance->GetCreature(m_uiTorsoGUID))
                if (pTorso && pTorso->isAlive())
                    pTorso->CastSpell(pTorso, SPELL_BERSERK, false);
            
            if (Creature* pHead = m_pInstance->instance->GetCreature(m_uiHeadGUID))
                if (pHead && pHead->isAlive())
                    pHead->CastSpell(pHead, SPELL_BERSERK, false);
            
            m_uiBerserkTimer = 330000;
        } 
    }
};

// Leviathan MK turret
// used in phase 1; should be attached by a vehicle seat to the Leviathan MK
struct MANGOS_DLL_DECL leviathan_turretAI : public ScriptedAI
{   
    leviathan_turretAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->SetVisibility(VISIBILITY_OFF);   
        //pCreature->setFaction(14);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiPlasmaBlastTimer;
    uint32 m_uiNapalmShellTimer;

    void Reset()
    {
        m_uiPlasmaBlastTimer = 20000;
        m_uiNapalmShellTimer = 10000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // turret abilities implemented in leviathan himself
        /*if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPlasmaBlastTimer < uiDiff)
        {
            if (Creature* pTank = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(NPC_LEVIATHAN_MK))))
            {
                DoScriptText(EMOTE_PLASMA_BLAST, m_creature);
                if (Unit* pTarget = pTank->SelectAttackingPlayer(ATTACKING_TARGET_TOPAGGRO, 0))
                    DoCast(pTarget, m_bIsRegularMode ? SPELL_PLASMA_BLAST : SPELL_PLASMA_BLAST_H);
            }
            m_uiPlasmaBlastTimer = 30000;
        }
        else m_uiPlasmaBlastTimer -= uiDiff;

        if (m_uiNapalmShellTimer < uiDiff)
        {
            if (Creature* pTank = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(NPC_LEVIATHAN_MK))))
            {
                if (Unit* pTarget = pTank->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (!pTank->IsWithinDistInMap(pTarget, 15))
                    {
                        DoCast(pTarget, m_bIsRegularMode ? SPELL_NAPALM_SHELL : SPELL_NAPALM_SHELL_H);
                        m_uiNapalmShellTimer = 7000;
                    }
                }
            }
        }
        else m_uiNapalmShellTimer -= uiDiff;*/
    }
};

struct MANGOS_DLL_DECL mob_proximity_mineAI : public ScriptedAI
{   
    mob_proximity_mineAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiDieTimer;
    bool m_bExploded;

    void Reset()
    {
        m_uiDieTimer        = 65000;
        m_bExploded         = false;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho || !pWho->IsHostileTo(m_creature) || m_bExploded)
            return;

        if (m_creature->IsWithinDistInMap(pWho, 3))
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_EXPLOSION : SPELL_EXPLOSION_H);
            m_bExploded = true;
            m_uiDieTimer = 500;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_MIMIRON) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiDieTimer < uiDiff)
        {
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            m_uiDieTimer = 50000;
        }else m_uiDieTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL mob_frost_bombAI : public ScriptedAI
{   
    mob_frost_bombAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->setFaction(14);
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance *pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiExplosionTimer;
    uint32 m_uiDieTimer;

    void Reset()
    {
        m_uiExplosionTimer  = 12000;
        m_uiDieTimer        = 15000;
        DoCast(m_creature, SPELL_FROST_BOMB_AURA);
    }

    void SuppressFiresInRange(uint32 range)
    {
        std::list<Creature*> lFires;
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34363, range);
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34121, range);
        if (!lFires.empty())
        {
            for(std::list<Creature*>::iterator iter = lFires.begin(); iter != lFires.end(); ++iter)
            {
                if ((*iter) && (*iter)->isAlive())
                    (*iter)->ForcedDespawn();
            }
        }
    }

    void CastFinished(const SpellEntry* spellInfo)
    {
        if (spellInfo->Id == SPELL_FROST_BOMB_EXPL)
            SuppressFiresInRange(30);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiDieTimer < uiDiff)
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        else
            m_uiDieTimer -= uiDiff;

        if (m_uiExplosionTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_FROST_BOMB_EXPL);
            m_uiExplosionTimer = 100000;
            m_uiDieTimer = 500;
        }
        else m_uiExplosionTimer -= uiDiff;
    }
};

// Flames used in hard mode
struct MANGOS_DLL_DECL mob_mimiron_flamesAI : public ScriptedAI
{   
    mob_mimiron_flamesAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->setFaction(14);
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiFlamesSpreadTimer;
    bool m_bSpreaded;

    void Reset()
    {
        DoCast(m_creature, SPELL_FLAMES);
        m_uiFlamesSpreadTimer = 4000;
        m_bSpreaded = false;
    }

    void AttackStart(Unit* pWho) { return; }
    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        uiDamage = 0;
    }
    Player* SelectNearestPlayer()
    {
        Player* nearest = NULL;
        float neardist = 999.0f;
        Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            Player* plr = itr->getSource();
            if (!plr)
                continue;

            float currdist = plr->GetDistance(m_creature);
            if (currdist < neardist)
            {
                nearest = plr;
                neardist = currdist;
            }
        }
        return nearest;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_MIMIRON) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (m_uiFlamesSpreadTimer < uiDiff && !m_bSpreaded)
        {
            Player* plr = SelectNearestPlayer();
            
            const float distance = 5;
            float angle = plr ? m_creature->GetAngle(plr) : 0;
            float x = m_creature->GetPositionX() + distance*cos(angle);
            float y = m_creature->GetPositionY() + distance*sin(angle);
            m_creature->SummonCreature(34121, x, y, CENTER_Z, 0, TEMPSUMMON_DEAD_DESPAWN, 0);			
            m_bSpreaded = true;
            m_uiFlamesSpreadTimer = 3000;
        }
        else m_uiFlamesSpreadTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL mob_mimiron_infernoAI : public ScriptedAI
{   
    mob_mimiron_infernoAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->setFaction(14);
        pCreature->SetDisplayId(11686);     // make invisible
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiFlamesTimer;

    void Reset()
    {
        DoCast(m_creature, SPELL_SELF_DESTRUCTION);
        m_uiFlamesTimer = 2000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_MIMIRON) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (m_uiFlamesTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_SELF_DESTRUCT);
            m_uiFlamesTimer = 1000;
        }
        else m_uiFlamesTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL mob_magnetic_coreAI : public ScriptedAI
{   
    mob_magnetic_coreAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiSpellTimer;
    uint32 m_uiDieTimer;

    void Reset()
    {
        m_uiSpellTimer = 2000;
        m_uiDieTimer = 23000;
        SetCombatMovement(false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_MIMIRON_PHASE) != PHASE_AERIAL)
        {
            m_creature->ForcedDespawn();
            return;
        }

        if (m_uiSpellTimer < uiDiff)
        {
            if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_AERIAL)
                if (Creature* pAerial = GetClosestCreatureWithEntry(m_creature, NPC_AERIAL_UNIT, 10.0f))
                    pAerial->CastSpell(pAerial, SPELL_MAGNETIC_CORE, true);

            m_uiSpellTimer = 100000;
        }
        else m_uiSpellTimer -= uiDiff;

        if (m_uiDieTimer < uiDiff)
            m_creature->ForcedDespawn();
        else
            m_uiDieTimer -= uiDiff;
    }
};

/////////////////////////////
///          BOTS         ///
/////////////////////////////
struct MANGOS_DLL_DECL mob_bomb_botAI : public ScriptedAI
{   
    mob_bomb_botAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiDieTimer;
    uint32 m_uiChangeTarget;
    bool m_bExploded;

    void Reset()
    {
        m_uiDieTimer        = 600000;
        m_uiChangeTarget    = 5000;
        m_bExploded         = false;

        if (Player* plr = SelectNearestPlayer())
            AttackStart(plr);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth())
        {
            DoCast(m_creature, SPELL_BOMB_BOT_SACRIFICE);
            m_bExploded = true;
            uiDamage = 0;
            m_uiDieTimer = 500;
        }
    }
    
    void MoveInLineOfSight(Unit* pWho)
    {
        // already exploded
        if (m_bExploded)
            return;

        if (Unit* pUnit = SelectNearestPlayer())
            if (m_creature->GetDistance(pUnit) < 5)
            {
                DoCast(m_creature, SPELL_BOMB_BOT_SACRIFICE);
                m_bExploded = true;
                m_uiDieTimer = 500;
            }
    }

    Player* SelectNearestPlayer()
    {
        Player* nearest = NULL;
        float neardist = 999.0f;
        Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            Player* plr = itr->getSource();
            if (!plr)
                continue;

            float currdist = plr->GetDistance(m_creature);
            if (currdist < neardist)
            {
                nearest = plr;
                neardist = currdist;
            }
        }
        return nearest;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiDieTimer < uiDiff)
        {
            //DoCast(m_creature, SPELL_BOMB_BOT_SACRIFICE);
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
        else
            m_uiDieTimer -= uiDiff;

        // after some time, check if some other target can be found
        if (HandleTimer(m_uiChangeTarget, uiDiff, true))
        {
            if (Player* plr = SelectNearestPlayer())
                AttackStart(plr);

            m_uiChangeTarget = 5000;
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_assault_botAI : public ScriptedAI
{   
    mob_assault_botAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiMagneticFieldTimer;

    void Reset()
    {
        m_uiMagneticFieldTimer = 5000;
        if (m_creature->GetCreator())
            AttackStart(m_creature->GetCreator()->getVictim());
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (HandleTimer(m_uiMagneticFieldTimer, uiDiff))
        {
            DoCast(m_creature->getVictim(), SPELL_MAGNETIC_FIELD);
            m_uiMagneticFieldTimer = urand(10000, 15000);
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_emergency_botAI : public ScriptedAI
{   
    mob_emergency_botAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 m_uiWaterSprayTimer;

    void Reset()
    {
        m_uiWaterSprayTimer = urand(5000, 10000);
        
        if (!m_bIsRegularMode)
            DoCast(m_creature, SPELL_DEAFENING_SIREN);

        DoWaterSpray();
    }

    std::list<Creature*> GetFiresInCone()
    {
        // all fires in 15 yards
        std::list<Creature*> lFires;
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34363, 15);
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34121, 15);

        std::list<Creature*> m_lProperFires;
        
        // fill list with 
        if (!lFires.empty())
            for (std::list<Creature*>::iterator itr = lFires.begin(); itr != lFires.end(); ++itr)
                if (m_creature->isInFront(*itr, 15, M_PI_F/12))
                    m_lProperFires.push_back(*itr);

        return m_lProperFires;
    }

    Creature* SelectRandomFireInRange(uint32 range)
    {
        std::list<Creature*> lFires;
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34363, range);
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34121, range);

        if (!lFires.empty())
        {
            std::list<Creature* >::iterator iter = lFires.begin();
            advance(iter, urand(0, lFires.size()-1));

            if ((*iter)->isAlive())
                return *iter;
        }

        return 0;
    }

    void AttackStart(Unit* pWho) {}
    void DoWaterSpray()
    {
        if (Creature* pFire = SelectRandomFireInRange(14))
        {
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->StopMoving();

            m_creature->SetInFront(pFire);

            DoCast(m_creature, SPELL_WATER_SPRAY);
            
            // should be implemented in core
            std::list<Creature*> m_lFires = GetFiresInCone();
            for(std::list<Creature*>::iterator itr = m_lFires.begin(); itr != m_lFires.end(); ++itr)
                (*itr)->ForcedDespawn();
        }
        else
        {
            // if no fire in 15 range then find closest one
            for (uint8 i = 15; i < 99; ++i)
            {
                if (Creature* pFire = SelectRandomFireInRange(i))
                    DoStartMovement(pFire, 10);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Water Spray
        if (m_uiWaterSprayTimer < uiDiff)
        {
            DoWaterSpray();
            m_uiWaterSprayTimer = 4000;
        }
        else m_uiWaterSprayTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

// Red button -> used to start the hard mode
/////////////////////////////
///       RED BUTTON      ///
/////////////////////////////
bool GOHello_go_red_button(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* m_pInstance = (ScriptedInstance*)pGo->GetInstanceData();

    if (!m_pInstance)
        return false;

    if (Creature* pMimiron = ((Creature*)Unit::GetUnit((*pGo), m_pInstance->GetData64(NPC_MIMIRON))))
    {
        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1); 
        if (pMimiron->isAlive() && m_pInstance->GetData(TYPE_MIMIRON_PHASE) <= PHASE_INTRO)
            ((boss_mimironAI*)pMimiron->AI())->m_bIsHardMode = true;
        pMimiron->AI()->AttackStart(pPlayer);
        m_pInstance->SetData(TYPE_MIMIRON_HARD, IN_PROGRESS);
    }

    return false;
}

CreatureAI* GetAI_boss_mimiron(Creature* pCreature)
{
    return new boss_mimironAI(pCreature);
}

CreatureAI* GetAI_boss_leviathan_mk(Creature* pCreature)
{
    return new boss_leviathan_mkAI(pCreature);
}

CreatureAI* GetAI_boss_vx001(Creature* pCreature)
{
    return new boss_vx001AI(pCreature);
}

CreatureAI* GetAI_boss_aerial_command_unit(Creature* pCreature)
{
    return new boss_aerial_command_unitAI(pCreature);
}

CreatureAI* GetAI_leviathan_turret(Creature* pCreature)
{
    return new leviathan_turretAI(pCreature);
}

CreatureAI* GetAI_mob_proximity_mine(Creature* pCreature)
{
    return new mob_proximity_mineAI(pCreature);
}

CreatureAI* GetAI_mob_bomb_bot(Creature* pCreature)
{
    return new mob_bomb_botAI(pCreature);
}

CreatureAI* GetAI_mob_assault_bot(Creature* pCreature)
{
    return new mob_assault_botAI(pCreature);
}

CreatureAI* GetAI_mob_emergency_bot(Creature* pCreature)
{
    return new mob_emergency_botAI(pCreature);
}

CreatureAI* GetAI_mob_frost_bomb(Creature* pCreature)
{
    return new mob_frost_bombAI(pCreature);
}

CreatureAI* GetAI_mob_mimiron_flames(Creature* pCreature)
{
    return new mob_mimiron_flamesAI(pCreature);
}

CreatureAI* GetAI_mob_mimiron_inferno(Creature* pCreature)
{
    return new mob_mimiron_infernoAI(pCreature);
}

CreatureAI* GetAI_mob_magnetic_core(Creature* pCreature)
{
    return new mob_magnetic_coreAI(pCreature);
}

void AddSC_boss_mimiron()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_mimiron";
    newscript->GetAI = &GetAI_boss_mimiron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_leviathan_mk";
    newscript->GetAI = &GetAI_boss_leviathan_mk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "leviathan_turret";
    newscript->GetAI = &GetAI_leviathan_turret;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_vx001";
    newscript->GetAI = &GetAI_boss_vx001;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_aerial_command_unit";
    newscript->GetAI = &GetAI_boss_aerial_command_unit;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_proximity_mine";
    newscript->GetAI = &GetAI_mob_proximity_mine;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_bomb_bot";
    newscript->GetAI = &GetAI_mob_bomb_bot;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_assault_bot";
    newscript->GetAI = &GetAI_mob_assault_bot;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_emergency_bot";
    newscript->GetAI = &GetAI_mob_emergency_bot;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_frost_bomb";
    newscript->GetAI = &GetAI_mob_frost_bomb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_mimiron_flames";
    newscript->GetAI = &GetAI_mob_mimiron_flames;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_mimiron_inferno";
    newscript->GetAI = &GetAI_mob_mimiron_inferno;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_magnetic_core";
    newscript->GetAI = &GetAI_mob_magnetic_core;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "go_red_button";
    newscript->pGOHello = &GOHello_go_red_button;
    newscript->RegisterSelf();
}

/*
UPDATE `creature_template` SET `LootID`= 34057 WHERE `entry` = 34057;
DELETE FROM `creature_loot_template` WHERE `entry` = 34057;
INSERT INTO `creature_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`Groupid`,`mincountOrRef`,`maxcount`,`lootcondition`,`condition_value1`,`condition_value2`) VALUES
(34057, 46029, 100, 0, 1, 1, 0, 0, 0);
INSERT IGNORE INTO spell_script_target VALUES (63274,1,34181), (63414, 1,34181);
UPDATE creature_template SET AIName = 'NullAI' WHERE entry = 34181;
UPDATE `creature_template` SET `InhabitType` = '7' WHERE `creature_template`.`entry` IN (33670,33651);
34147 staty
-- Normal
UPDATE `creature_template` SET `mindmg` = 4000, `maxdmg` = 5000, `resistance1` = 25, `resistance2` = 25, `resistance3` = 25, `resistance4` = 25, `resistance5` = 25, `resistance6` = 25, `dmg_multiplier` = 1,armor= 8000,minhealth=12600,maxhealth=12600 WHERE `entry` = 34147;
-- 25
UPDATE `creature_template` SET `mindmg` = 4000, `maxdmg` = 5000, `resistance1` = 25, `resistance2` = 25, `resistance3` = 25, `resistance4` = 25, `resistance5` = 25, `resistance6` = 25, `dmg_multiplier` = 1,armor= 8000,minhealth=50400,maxhealth=50400 WHERE `entry` = 34148;
*/
