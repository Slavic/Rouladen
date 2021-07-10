#pragma once
#include "pch.h"
#include "geom.h"


/* An SDK-esque header file which contains structs and other declarations for the Cube engine
*  Everything except variables, enums, and virtual functions has been removed unless specifically required
*  Including contructors as we never create structs that aren't pointers, hence the swath of warnings
*/ 


namespace Cube
{
    enum { GUN_FIST = 0, GUN_SG, GUN_CG, GUN_RL, GUN_RIFLE, GUN_GL, GUN_PISTOL, GUN_FIREBALL, GUN_ICEBALL, GUN_SLIMEBALL, GUN_BITE, GUN_BARREL, NUMGUNS };
    enum { A_BLUE, A_GREEN, A_YELLOW };     // armour types... take 20/40/60 % off
    enum { M_NONE = 0, M_SEARCH, M_HOME, M_ATTACKING, M_PAIN, M_SLEEP, M_AIMING };  // monster states

    static struct itemstat { int add, max, sound; const char* name; int icon, info; } itemstats[11];
    enum { ET_EMPTY = 0, ET_LIGHT, ET_MAPMODEL, ET_PLAYERSTART, ET_ENVMAP, ET_PARTICLES, ET_SOUND, ET_SPOTLIGHT, ET_GAMESPECIFIC };

    enum                            // static entity types
    {
        NOTUSED = ET_EMPTY,         // entity slot not in use in map
        LIGHT = ET_LIGHT,           // lightsource, attr1 = radius, attr2 = intensity
        MAPMODEL = ET_MAPMODEL,     // attr1 = angle, attr2 = idx
        PLAYERSTART,                // attr1 = angle, attr2 = team
        ENVMAP = ET_ENVMAP,         // attr1 = radius
        PARTICLES = ET_PARTICLES,
        MAPSOUND = ET_SOUND,
        SPOTLIGHT = ET_SPOTLIGHT,
        I_SHELLS, I_BULLETS, I_ROCKETS, I_ROUNDS, I_GRENADES, I_CARTRIDGES,
        I_HEALTH, I_BOOST,
        I_GREENARMOUR, I_YELLOWARMOUR,
        I_QUAD,
        TELEPORT,                   // attr1 = idx, attr2 = model, attr3 = tag
        TELEDEST,                   // attr1 = angle, attr2 = idx
        MONSTER,                    // attr1 = angle, attr2 = monstertype
        CARROT,                     // attr1 = tag, attr2 = type
        JUMPPAD,                    // attr1 = zpush, attr2 = ypush, attr3 = xpush
        BASE,
        RESPAWNPOINT,
        BOX,                        // attr1 = angle, attr2 = idx, attr3 = weight
        BARREL,                     // attr1 = angle, attr2 = idx, attr3 = weight, attr4 = health
        PLATFORM,                   // attr1 = angle, attr2 = idx, attr3 = tag, attr4 = speed
        ELEVATOR,                   // attr1 = angle, attr2 = idx, attr3 = tag, attr4 = speed
        FLAG,                       // attr1 = angle, attr2 = team
        MAXENTTYPES
    };

    struct physent                                  // base entity type, can be affected by physics
    {
        vec o, vel, falling;                        // origin, velocity
        vec deltapos, newpos;                       // movement interpolation
        float yaw, pitch, roll;
        float maxspeed;                             // cubes per second, 100 for player
        float radius, eyeheight, aboveeye;          // bounding box size
        float xradius, yradius, zmargin;
        vec floor;                                  // the normal of floor the dynent is on

        unsigned short timeinair;
        unsigned char inwater;
        bool jumping;
        signed char move, strafe;

        unsigned char physstate;                            // one of PHYS_* above
        unsigned char state, editstate;                     // one of CS_* above
        unsigned char type;                                 // one of ENT_* above
        unsigned char collidetype;                          // one of COLLIDE_* above           

        bool blocked;                               // used by physics to signal ai

    };

    enum
    {
        ANIM_DEAD = 0, ANIM_DYING, ANIM_IDLE,
        ANIM_FORWARD, ANIM_BACKWARD, ANIM_LEFT, ANIM_RIGHT,
        ANIM_HOLD1, ANIM_HOLD2, ANIM_HOLD3, ANIM_HOLD4, ANIM_HOLD5, ANIM_HOLD6, ANIM_HOLD7,
        ANIM_ATTACK1, ANIM_ATTACK2, ANIM_ATTACK3, ANIM_ATTACK4, ANIM_ATTACK5, ANIM_ATTACK6, ANIM_ATTACK7,
        ANIM_PAIN,
        ANIM_JUMP, ANIM_SINK, ANIM_SWIM,
        ANIM_EDIT, ANIM_LAG, ANIM_TAUNT, ANIM_WIN, ANIM_LOSE,
        ANIM_GUN_IDLE, ANIM_GUN_SHOOT,
        ANIM_VWEP_IDLE, ANIM_VWEP_SHOOT, ANIM_SHIELD, ANIM_POWERUP,
        ANIM_MAPMODEL, ANIM_TRIGGER,
        NUMANIMS
    };

    static const char* const animnames[] =
    {
        "dead", "dying", "idle",
        "forward", "backward", "left", "right",
        "hold 1", "hold 2", "hold 3", "hold 4", "hold 5", "hold 6", "hold 7",
        "attack 1", "attack 2", "attack 3", "attack 4", "attack 5", "attack 6", "attack 7",
        "pain",
        "jump", "sink", "swim",
        "edit", "lag", "taunt", "win", "lose",
        "gun idle", "gun shoot",
        "vwep idle", "vwep shoot", "shield", "powerup",
        "mapmodel", "trigger"
    };

#define ANIM_ALL         0x7F
#define ANIM_INDEX       0x7F
#define ANIM_LOOP        (1<<7)
#define ANIM_START       (1<<8)
#define ANIM_END         (1<<9)
#define ANIM_REVERSE     (1<<10)
#define ANIM_CLAMP       (ANIM_START|ANIM_END)
#define ANIM_DIR         0x780
#define ANIM_SECONDARY   11
#define ANIM_NOSKIN      (1<<22)
#define ANIM_SETTIME     (1<<23)
#define ANIM_FULLBRIGHT  (1<<24)
#define ANIM_REUSE       (1<<25)
#define ANIM_NORENDER    (1<<26)
#define ANIM_RAGDOLL     (1<<27)
#define ANIM_SETSPEED    (1<<28)
#define ANIM_NOPITCH     (1<<29)
#define ANIM_GHOST       (1<<30)
#define ANIM_FLAGS       (0x1FF<<22)

    struct animinfo // description of a character's animation
    {
        int anim, frame, range, basetime;
        float speed;
        unsigned int varseed;

    };

    struct animinterpinfo // used for animation blending of animated characters
    {
        animinfo prev, cur;
        int lastswitch;
        void* lastmodel;

    };

#define MAXANIMPARTS 3
#define MAXSTRLEN 260
    typedef char string[MAXSTRLEN];

    struct occludequery;
    struct ragdolldata;

    struct entitylight
    {
        vec color, dir;
        int millis;
    };

    struct dynent : physent                         // animated characters, or characters that can receive input
    {
        bool k_left, k_right, k_up, k_down;         // see input code

        entitylight light;
        animinterpinfo animinterp[MAXANIMPARTS];
        ragdolldata* ragdoll;
        occludequery* query;
        int lastrendered;
        unsigned char occluded;

    };
    struct fpsstate
    {
        int health, maxhealth;
        int armour, armourtype;
        int quadmillis;
        int gunselect, gunwait;
        int ammo[NUMGUNS];
        int aitype, skill;

    
    };

    struct fpsent : dynent, fpsstate
    {
        int weight;                         // affects the effectiveness of hitpush
        int clientnum, privilege, lastupdate, plag, ping;
        int lifesequence;                   // sequence id for each respawn, used in damage test
        int respawned, suicided;
        int lastpain;
        int lastaction, lastattackgun;
        bool attacking;
        int attacksound, attackchan, idlesound, idlechan;
        int lasttaunt;
        int lastpickup, lastpickupmillis, lastbase, lastrepammo, flagpickup, tokens;
        vec lastcollect;
        int frags, flags, deaths, totaldamage, totalshots;
        void* edit;
        float deltayaw, deltapitch, deltaroll, newyaw, newpitch, newroll;
        int smoothmillis;

        
        string name, team, info;
        int playermodel;
        void* ai;
        int ownernum, lastnode;

        vec muzzle;

    };

    
    struct server_entity            // server side version of "entity" type
    {
        int type;
        int spawntime;
        bool spawned;
    };

    struct playerList
    {
        Cube::fpsent* players[32];
        int alen;
        int len;
    };

    template <class T> struct vector
    {
        static const int MINSIZE = 8;

        T* buf;
        int alen, ulen;

        vector() : buf(NULL), alen(0), ulen(0)
        {
        }

        vector(const vector& v) : buf(NULL), alen(0), ulen(0)
        {
            *this = v;
        }


        bool inrange(size_t i) const { return i < size_t(ulen); }
        bool inrange(int i) const { return i >= 0 && i < ulen; }

       
        bool empty() const { return ulen == 0; }

        int capacity() const { return alen; }
        int length() const { return ulen; }
        T& operator[](int i) { return buf[i]; }
        const T& operator[](int i) const { return buf[i]; }
    };


    struct model;
    struct modelattach
    {
        const char* tag, * name;
        int anim, basetime;
        vec* pos;
        model* m;
    };

    struct playermodelinfo
    {
        const char* ffa, * blueteam, * redteam, * hudguns,
            * vwep, * quad, * armour[3],
            * ffaicon, * blueicon, * redicon;
        bool ragdoll;
    };

    enum
    {
        M_TEAM = 1 << 0,
        M_NOITEMS = 1 << 1,
        M_NOAMMO = 1 << 2,
        M_INSTA = 1 << 3,
        M_EFFICIENCY = 1 << 4,
        M_TACTICS = 1 << 5,
        M_CAPTURE = 1 << 6,
        M_REGEN = 1 << 7,
        M_CTF = 1 << 8,
        M_PROTECT = 1 << 9,
        M_HOLD = 1 << 10,
        M_EDIT = 1 << 12,
        M_DEMO = 1 << 13,
        M_LOCAL = 1 << 14,
        M_LOBBY = 1 << 15,
        M_DMSP = 1 << 16,
        M_CLASSICSP = 1 << 17,
        M_SLOWMO = 1 << 18,
        M_COLLECT = 1 << 19
    };

    static struct gamemodeinfo
    {
        const char* name;
        int flags;
        const char* info;
    } gamemodes[] =
    {
        { "SP", M_LOCAL | M_CLASSICSP, NULL },
        { "DMSP", M_LOCAL | M_DMSP, NULL },
        { "demo", M_DEMO | M_LOCAL, NULL},
        { "ffa", M_LOBBY, "Free For All: Collect items for ammo. Frag everyone to score points." },
        { "coop edit", M_EDIT, "Cooperative Editing: Edit maps with multiple players simultaneously." },
        { "teamplay", M_TEAM, "Teamplay: Collect items for ammo. Frag \fs\f3the enemy team\fr to score points for \fs\f1your team\fr." },
        { "instagib", M_NOITEMS | M_INSTA, "Instagib: You spawn with full rifle ammo and die instantly from one shot. There are no items. Frag everyone to score points." },
        { "insta team", M_NOITEMS | M_INSTA | M_TEAM, "Instagib Team: You spawn with full rifle ammo and die instantly from one shot. There are no items. Frag \fs\f3the enemy team\fr to score points for \fs\f1your team\fr." },
        { "efficiency", M_NOITEMS | M_EFFICIENCY, "Efficiency: You spawn with all weapons and armour. There are no items. Frag everyone to score points." },
        { "effic team", M_NOITEMS | M_EFFICIENCY | M_TEAM, "Efficiency Team: You spawn with all weapons and armour. There are no items. Frag \fs\f3the enemy team\fr to score points for \fs\f1your team\fr." },
        { "tactics", M_NOITEMS | M_TACTICS, "Tactics: You spawn with two random weapons and armour. There are no items. Frag everyone to score points." },
        { "tac team", M_NOITEMS | M_TACTICS | M_TEAM, "Tactics Team: You spawn with two random weapons and armour. There are no items. Frag \fs\f3the enemy team\fr to score points for \fs\f1your team\fr." },
        { "capture", M_NOAMMO | M_TACTICS | M_CAPTURE | M_TEAM, "Capture: Capture neutral bases or steal \fs\f3enemy bases\fr by standing next to them.  \fs\f1Your team\fr scores points for every 10 seconds it holds a base. You spawn with two random weapons and armour. Collect extra ammo that spawns at \fs\f1your bases\fr. There are no ammo items." },
        { "regen capture", M_NOITEMS | M_CAPTURE | M_REGEN | M_TEAM, "Regen Capture: Capture neutral bases or steal \fs\f3enemy bases\fr by standing next to them. \fs\f1Your team\fr scores points for every 10 seconds it holds a base. Regenerate health and ammo by standing next to \fs\f1your bases\fr. There are no items." },
        { "ctf", M_CTF | M_TEAM, "Capture The Flag: Capture \fs\f3the enemy flag\fr and bring it back to \fs\f1your flag\fr to score points for \fs\f1your team\fr. Collect items for ammo." },
        { "insta ctf", M_NOITEMS | M_INSTA | M_CTF | M_TEAM, "Instagib Capture The Flag: Capture \fs\f3the enemy flag\fr and bring it back to \fs\f1your flag\fr to score points for \fs\f1your team\fr. You spawn with full rifle ammo and die instantly from one shot. There are no items." },
        { "protect", M_CTF | M_PROTECT | M_TEAM, "Protect The Flag: Touch \fs\f3the enemy flag\fr to score points for \fs\f1your team\fr. Pick up \fs\f1your flag\fr to protect it. \fs\f1Your team\fr loses points if a dropped flag resets. Collect items for ammo." },
        { "insta protect", M_NOITEMS | M_INSTA | M_CTF | M_PROTECT | M_TEAM, "Instagib Protect The Flag: Touch \fs\f3the enemy flag\fr to score points for \fs\f1your team\fr. Pick up \fs\f1your flag\fr to protect it. \fs\f1Your team\fr loses points if a dropped flag resets. You spawn with full rifle ammo and die instantly from one shot. There are no items." },
        { "hold", M_CTF | M_HOLD | M_TEAM, "Hold The Flag: Hold \fs\f7the flag\fr for 20 seconds to score points for \fs\f1your team\fr. Collect items for ammo." },
        { "insta hold", M_NOITEMS | M_INSTA | M_CTF | M_HOLD | M_TEAM, "Instagib Hold The Flag: Hold \fs\f7the flag\fr for 20 seconds to score points for \fs\f1your team\fr. You spawn with full rifle ammo and die instantly from one shot. There are no items." },
        { "effic ctf", M_NOITEMS | M_EFFICIENCY | M_CTF | M_TEAM, "Efficiency Capture The Flag: Capture \fs\f3the enemy flag\fr and bring it back to \fs\f1your flag\fr to score points for \fs\f1your team\fr. You spawn with all weapons and armour. There are no items." },
        { "effic protect", M_NOITEMS | M_EFFICIENCY | M_CTF | M_PROTECT | M_TEAM, "Efficiency Protect The Flag: Touch \fs\f3the enemy flag\fr to score points for \fs\f1your team\fr. Pick up \fs\f1your flag\fr to protect it. \fs\f1Your team\fr loses points if a dropped flag resets. You spawn with all weapons and armour. There are no items." },
        { "effic hold", M_NOITEMS | M_EFFICIENCY | M_CTF | M_HOLD | M_TEAM, "Efficiency Hold The Flag: Hold \fs\f7the flag\fr for 20 seconds to score points for \fs\f1your team\fr. You spawn with all weapons and armour. There are no items." },
        { "collect", M_COLLECT | M_TEAM, "Skull Collector: Frag \fs\f3the enemy team\fr to drop \fs\f3skulls\fr. Collect them and bring them to \fs\f3the enemy base\fr to score points for \fs\f1your team\fr or steal back \fs\f1your skulls\fr. Collect items for ammo." },
        { "insta collect", M_NOITEMS | M_INSTA | M_COLLECT | M_TEAM, "Instagib Skull Collector: Frag \fs\f3the enemy team\fr to drop \fs\f3skulls\fr. Collect them and bring them to \fs\f3the enemy base\fr to score points for \fs\f1your team\fr or steal back \fs\f1your skulls\fr. You spawn with full rifle ammo and die instantly from one shot. There are no items." },
        { "effic collect", M_NOITEMS | M_EFFICIENCY | M_COLLECT | M_TEAM, "Efficiency Skull Collector: Frag \fs\f3the enemy team\fr to drop \fs\f3skulls\fr. Collect them and bring them to \fs\f3the enemy base\fr to score points for \fs\f1your team\fr or steal back \fs\f1your skulls\fr. You spawn with all weapons and armour. There are no items." }
    };

    struct GlobalShaderParamState
    {
        const char* name;
        union
        {
            float fval[32];
            int ival[32];
            unsigned int uval[32];
            unsigned char buf[32 * sizeof(float)];
        };
        int version;

        static int nextversion;
    };

    struct ShaderParamBinding
    {
        int loc, size;
        GLenum format;
    };

    struct GlobalShaderParamUse : ShaderParamBinding
    {

        GlobalShaderParamState* param;
        int version;
    };

    struct LocalShaderParamState : ShaderParamBinding
    {
        const char* name;
    };

    struct SlotShaderParam
    {
        const char* name;
        int loc;
        float val[4];
    };

    struct SlotShaderParamState : LocalShaderParamState
    {
        float val[4];
        
    };

    enum
    {
        SHADER_DEFAULT = 0,
        SHADER_NORMALSLMS = 1 << 0,
        SHADER_ENVMAP = 1 << 1,
        SHADER_OPTION = 1 << 3,

        SHADER_INVALID = 1 << 8,
        SHADER_DEFERRED = 1 << 9
    };

#define MAXSHADERDETAIL 3
#define MAXVARIANTROWS 5

    extern int shaderdetail;

    struct Slot;
    struct VSlot;

    struct UniformLoc
    {
        const char* name, * blockname;
        int loc, version, binding, stride, offset, size;
        void* data;
    };

    struct AttribLoc
    {
        const char* name;
        int loc;
    };

    struct Shader
    {
        static Shader* lastshader;

        char* name, * vsstr, * psstr, * defer;
        int type;
        GLuint program, vsobj, psobj;
        vector<SlotShaderParamState> defaultparams;
        vector<GlobalShaderParamUse> globalparams;
        vector<LocalShaderParamState> localparams;
        vector<unsigned char> localparamremap;
        Shader* detailshader, * variantshader, * altshader, * fastshader[MAXSHADERDETAIL];
        vector<Shader*> variants;
        unsigned short* variantrows;
        bool standard, forced, used;
        Shader* reusevs, * reuseps;
        vector<UniformLoc> uniformlocs;
        vector<AttribLoc> attriblocs;
        const void* owner;
    };

    struct Texture
    {
        enum
        {
            IMAGE = 0,
            CUBEMAP = 1,
            TYPE = 0xFF,

            STUB = 1 << 8,
            TRANSIENT = 1 << 9,
            COMPRESSED = 1 << 10,
            ALPHA = 1 << 11,
            MIRROR = 1 << 12,
            FLAGS = 0xFF00
        };

        char* name;
        int type, w, h, xs, ys, bpp, clamp;
        bool mipmap, canreduce;
        GLuint id;
        unsigned char* alphamask;

    };
    enum { MDL_CULL_VFC = 1 << 0, MDL_CULL_DIST = 1 << 1, MDL_CULL_OCCLUDED = 1 << 2, MDL_CULL_QUERY = 1 << 3, MDL_SHADOW = 1 << 4, MDL_DYNSHADOW = 1 << 5, MDL_LIGHT = 1 << 6, MDL_DYNLIGHT = 1 << 7, MDL_FULLBRIGHT = 1 << 8, MDL_NORENDER = 1 << 9, MDL_LIGHT_FAST = 1 << 10, MDL_HUD = 1 << 11, MDL_GHOST = 1 << 12 };
    struct model
    {
        char* name;
        float spinyaw, spinpitch, offsetyaw, offsetpitch;
        bool collide, ellipsecollide, shadow, alphadepth, depthoffset;
        float scale;
        vec translate;
        void* bih;
        vec bbcenter, bbradius, bbextend, collidecenter, collideradius;
        float rejectradius, eyeheight, collidexyradius, collideheight;
        int batch;

        model(const char* name) {}
        virtual ~model() {  }
        virtual void calcbb(vec& center, vec& radius) = 0;
        virtual void render(int anim, int basetime, int basetime2, const vec& o, float yaw, float pitch, dynent* d, modelattach* a = NULL, const vec& color = vec(0, 0, 0), const vec& dir = vec(0, 0, 0), float transparent = 1) = 0;
        virtual bool load() = 0;
        virtual int type() const = 0;
        virtual void* setBIH() {  }
        virtual bool envmapped() {  }
        virtual bool skeletal() const {  }

        virtual void setshader(Shader* shader) {}
        virtual void setenvmap(float envmapmin, float envmapmax, Texture* envmap) {}
        virtual void setspec(float spec) {}
        virtual void setambient(float ambient) {}
        virtual void setglow(float glow, float glowdelta, float glowpulse) {}
        virtual void setglare(float specglare, float glowglare) {}
        virtual void setalphatest(float alpha) {}
        virtual void setalphablend(bool blend) {}
        virtual void setfullbright(float fullbright) {}
        virtual void setcullface(bool cullface) {}

        virtual void preloadBIH() { }
        virtual void preloadshaders(bool force = false) {}
        virtual void preloadmeshes() {}
        virtual void cleanup() {}

        virtual void startrender() {}
        virtual void endrender() {}

        void boundbox(vec& center, vec& radius)
        {
            if (bbradius.x < 0)
            {
                calcbb(bbcenter, bbradius);
                bbradius += bbextend;
            }
            center = bbcenter;
            radius = bbradius;
        }

        float collisionbox(vec& center, vec& radius)
        {
            if (collideradius.x < 0)
            {
                boundbox(collidecenter, collideradius);
                if (collidexyradius)
                {
                    collidecenter.x = collidecenter.y = 0;
                    collideradius.x = collideradius.y = collidexyradius;
                }
                if (collideheight)
                {
                    collidecenter.z = collideradius.z = collideheight / 2;
                }
                rejectradius = (vec(collidecenter).abs() += (collideradius)).Length();
            }
            center = collidecenter;
            radius = collideradius;
            return rejectradius;
        }

        float boundsphere(vec& center)
        {
            vec radius;
            boundbox(center, radius);
            return radius.Length();
        }

        float above()
        {
            vec center, radius;
            boundbox(center, radius);
            return center.z + radius.z;
        }
    };

    struct guninfo { int sound, attackdelay, damage, spread, projspeed, kickamount, range, rays, hitpush, exprad, ttl; const char* name, * file; short part; };


    struct entity                                   // persistent map entity
    {
        vec o;                                      // position
        short attr1, attr2, attr3, attr4, attr5;
        unsigned char type;                                 // type is one of the above
        unsigned char reserved;
    };

    enum
    {
        EF_NOVIS = 1 << 0,
        EF_NOSHADOW = 1 << 1,
        EF_NOCOLLIDE = 1 << 2,
        EF_ANIM = 1 << 3,
        EF_OCTA = 1 << 4,
        EF_RENDER = 1 << 5,
        EF_SOUND = 1 << 6,
        EF_SPAWNED = 1 << 7,
        EF_NOPICKUP = 1 << 8
    };

    struct extentity : entity                       // part of the entity that doesn't get saved to disk
    {
        int flags;  // the only dynamic state of a map entity
        entitylight light;
        extentity* attached;

        bool spawned() const { return (flags & EF_SPAWNED) != 0; }

        bool nopickup() const { return (flags & EF_NOPICKUP) != 0; }
    };

    static const playermodelinfo playermodels[5] =
    {
        { "mrfixit", "mrfixit/blue", "mrfixit/red", "mrfixit/hudguns", NULL, "mrfixit/horns", { "mrfixit/armor/blue", "mrfixit/armor/green", "mrfixit/armor/yellow" }, "mrfixit", "mrfixit_blue", "mrfixit_red", true },
        { "snoutx10k", "snoutx10k/blue", "snoutx10k/red", "snoutx10k/hudguns", NULL, "snoutx10k/wings", { "snoutx10k/armor/blue", "snoutx10k/armor/green", "snoutx10k/armor/yellow" }, "snoutx10k", "snoutx10k_blue", "snoutx10k_red", true },
        //{ "ogro/green", "ogro/blue", "ogro/red", "mrfixit/hudguns", "ogro/vwep", NULL, { NULL, NULL, NULL }, "ogro", "ogro_blue", "ogro_red", false },
        { "ogro2", "ogro2/blue", "ogro2/red", "mrfixit/hudguns", NULL, "ogro2/quad", { "ogro2/armor/blue", "ogro2/armor/green", "ogro2/armor/yellow" }, "ogro", "ogro_blue", "ogro_red", true },
        { "inky", "inky/blue", "inky/red", "inky/hudguns", NULL, "inky/quad", { "inky/armor/blue", "inky/armor/green", "inky/armor/yellow" }, "inky", "inky_blue", "inky_red", true },
        { "captaincannon", "captaincannon/blue", "captaincannon/red", "captaincannon/hudguns", NULL, "captaincannon/quad", { "captaincannon/armor/blue", "captaincannon/armor/green", "captaincannon/armor/yellow" }, "captaincannon", "captaincannon_blue", "captaincannon_red", true }
    };

    struct ivec2
    {
        union
        {
            struct { int x, y; };
            int v[2];
        };
        
    };

    struct vec2
    {
        union
        {
            struct { float x, y; };
            float v[2];
        };
    };

    struct ImageData
    {
        int w, h, bpp, levels, align, pitch;
        GLenum compressed;
        unsigned char* data;
        void* owner;
        void (*freefunc)(void*);
    };

    struct VSlot
    {
        Slot* slot;
        VSlot* next;
        int index, changed;
        vector<SlotShaderParam> params;
        bool linked;
        float scale;
        int rotation;
        ivec2 offset;
        vec2 scroll;
        int layer;
        float alphafront, alphaback;
        vec colorscale;
        vec glowcolor;
    };

    struct Slot
    {
        struct Tex
        {
            int type;
            Texture* t;
            string name;
            int combined;
        };

        int index;
        vector<Tex> sts;
        Shader* shader;
        vector<SlotShaderParam> params;
        VSlot* variants;
        bool loaded;
        unsigned int texmask;
        char* autograss;
        Texture* grasstex, * thumbnail;
        char* layermaskname;
        int layermaskmode;
        float layermaskscale;
        ImageData* layermask;

    };

    struct MSlot : Slot, VSlot
    {};
    
    struct animmodel : model
    {
        struct animspec
        {
            int frame, range;
            float speed;
            int priority;
        };

        struct animpos
        {
            int anim, fr1, fr2;
            float t;
        };

        struct part;

        struct animstate
        {
            part* owner;
            animpos cur, prev;
            float interp;

        };

        struct linkedpart;
        struct mesh;

        struct shaderparams
        {
            float spec, ambient, glow, glowdelta, glowpulse, specglare, glowglare, fullbright, envmapmin, envmapmax, scrollu, scrollv, alphatest;
        };

        struct shaderparamskey;

        struct skin : shaderparams
        {
            part* owner;
            Texture* tex, * masks, * envmap, * normalmap;
            Shader* shader;
            bool alphablend, cullface;
            shaderparamskey* key;


        };

        struct meshgroup;

        struct mesh
        {
            meshgroup* group;
            char* name;
            bool noclip;

            mesh() : group(NULL), name(NULL), noclip(false)
            {
            }

            virtual ~mesh()
            {
               
            }

            virtual void calcbb() {}

            virtual void genBIH() {}
           

            virtual void setshader(Shader* s)
            {
              
            }


            
        };

        struct meshgroup
        {
            meshgroup* next;
            int shared;
            char* name;
            vector<mesh*> meshes;

            virtual ~meshgroup()
            {
                
            }

            virtual int findtag(const char* name) { }
            virtual void concattagtransform() {}

            

            virtual void* animkey() {  }
            virtual int totalframes() const {  }
        

            virtual void cleanup() {}
            virtual void preload(part* p) {}
            virtual void render(const animstate* as, float pitch, const vec& axis, const vec& forward, dynent* d, part* p) {}

        };

        virtual meshgroup* loadmeshes(const char* name, va_list args) { return NULL; }

        struct linkedpart
        {
            part* p;
            int tag, anim, basetime;
            vec translate;
            vec* pos;
            float matrix[16];
        };

        struct part
        {
            animmodel* model;
            int index;
            meshgroup* meshes;
            vector<linkedpart> links;
            vector<skin> skins;
            vector<animspec>* anims[MAXANIMPARTS];
            int numanimparts;
            float pitchscale, pitchoffset, pitchmin, pitchmax;
            vec translate;

            virtual ~part()
            {

            }

            virtual void cleanup()
            {

            }

            virtual void getdefaultanim(animinfo& info, int anim, unsigned int varseed, dynent* d)
            {
 
            }


            virtual void loaded()
            {
               
            }
        };

        enum
        {
            LINK_TAG = 0,
            LINK_COOP,
            LINK_REUSE
        };

        virtual int linktype(animmodel* m) const { return LINK_TAG; }

        vector<part*> parts;
       
        virtual void flushpart() {}

       

        virtual bool flipy() const { return false; }
        virtual bool loadconfig() { return false; }
        virtual bool loaddefaultparts() { return false; }
        virtual void startload() {}
        virtual void endload() {}

        

        virtual void loaded()
        {
            
        }

    
    };
}
