#pragma once
#include "../../SDK/sdk.h"

// Nickname list, add other ones freely
std::vector<std::string> weird_nicknames =
{
    xorstr_("viper_rgoimpact"), xorstr_("lav25_sniper"), xorstr_("abrams_redeploy"), xorstr_("redeploy_jeepstuff"),
    xorstr_("mortar_lethal"), xorstr_("rus_aek971KILLA"), xorstr_("ucav_burst"), xorstr_("aek971JEEP"), xorstr_("shotgun7_AEK971"),
    xorstr_("lethal_rgoimpact11"), xorstr_("shotgun_REDPLOYX"), xorstr_("GhostBF420"), xorstr_("m1abrams_smaw33"),
    xorstr_("aek971_redeployER"), xorstr_("bf4_LETHALz"), xorstr_("phantombow_ucAv17"), xorstr_("mortar_ucAv48"),
    xorstr_("KillerM1ABRAMS"), xorstr_("870MCS_FraGGER"), xorstr_("phantom_REDPLOYER"), xorstr_("SPEC OPS_shotgun"),
    xorstr_("VIPER27_RUS"), xorstr_("ucav_KILLER88"), xorstr_("bf4_is_LIFE"), xorstr_("abrams_KILLA"), xorstr_("rpg_smaw45"),
    xorstr_("870MCS_VATNIK"), xorstr_("GHOSTFRAGGER"), xorstr_("burst_BURSTER"), xorstr_("lethal_FRAG10"),
    xorstr_("SPEC OPS_PHANTOM"), xorstr_("marksman_GHOST"), xorstr_("redeploy_VATNIK14"), xorstr_("phantoMGHOST"),
    xorstr_("M1ABRAMS_RUSKILLER"), xorstr_("SPEC_VIPER"), xorstr_("mortar_SNIPER1"), xorstr_("RUS_VATNIK007"),
    xorstr_("LAV25_M1ABRAMS"), xorstr_("mortar_BURST21"), xorstr_("frag_RUS"), xorstr_("rpg_SPECIALIST"),
    xorstr_("RUS_MARKSMAN7"), xorstr_("870MCS_SPEC OPS"), xorstr_("lethal_JEEPSTUFF"), xorstr_("phantom_SMAW"),
    xorstr_("RGOIMPACT_abrams"), xorstr_("GHOST_SPECIALIST"), xorstr_("redeploy_RGO12"), xorstr_("aek971_MORTAR"),
    xorstr_("ucav_FRAGBOMB"), xorstr_("870MCS_MORTAR"), xorstr_("SPECOPS_MORTAR"), xorstr_("ucav_UCAV"),
    xorstr_("M1ABRAMS_BF4"), xorstr_("SMAW_GHOST"), xorstr_("redeploy_BF4ER"), xorstr_("abrams_FRAGGER"),
    xorstr_("bf4_JEEPSTUFF"), xorstr_("sniper_VIPER"), xorstr_("frag_870MCS"), xorstr_("SMAW_REDPLOYER"),
    xorstr_("VATNIK_PHANTOM"), xorstr_("SPEC OPS_SMAW"), xorstr_("frag_AEK971"), xorstr_("JEEPSTUFF_BURST"),
    xorstr_("VATNIK_AEK971"), xorstr_("VIPER_VATNIK"), xorstr_("abrams_JEEP"), xorstr_("M1ABRAMS_VATNIK"),
    xorstr_("frag_PHANTOM"), xorstr_("phantom_JEEP"), xorstr_("SMAW_FRAGS"), xorstr_("M1ABRAMS_PHANTOM"),
    xorstr_("SMAW_870MCS"), xorstr_("KILLER_RPG"), xorstr_("GHOST_MARKSMAN"), xorstr_("ucav_ABRAMS"),
    xorstr_("VIPER_REDPLOY2"), xorstr_("burst_PHANTOM"), xorstr_("USMC_Doomguy"), xorstr_("Doomguy_noob"),
    xorstr_("PLmarine"), xorstr_("German_engineer"), xorstr_("noob_killerUA"), xorstr_("Monkey_Marksman"),
    xorstr_("US_specialist"), xorstr_("APE_M1ABRAMS"), xorstr_("LAV25_specialist"), xorstr_("phantombow_us"),
    xorstr_("DOOMGUYPL"), xorstr_("ua_frag"), xorstr_("rpg_specialist"), xorstr_("Marine_Monkey"),
    xorstr_("LMG_SPRAYER420"), xorstr_("DeSeRT_EAGLE_Ace"), xorstr_("pdw_RushERrr"), xorstr_("Carbine_VETERAN7"), xorstr_("PISTOL_KING1337"),
    xorstr_("C4_SpecialistX"), xorstr_("DeFuSeKiNG"), xorstr_("CLAYMORE_MASTERmind"), xorstr_("ShOtGuN_Brawler9000"), xorstr_("SNIPER_ELITE_"),
    xorstr_("TaNk_Commander"), xorstr_("HeLiCoPtEr_PiLoT"), xorstr_("JeT_FiGhTeR19"), xorstr_("APC_DriVeR"), xorstr_("BoAt_CaPtAiN_"),
    xorstr_("AgGrEsSiVe_RuShEr"), xorstr_("Tactical_DefenderX"), xorstr_("ObJeCtIvE_PLAYER_"), xorstr_("SuppOrt_Specialist7"),
    xorstr_("StEaLtHy_FlAnKeR"), xorstr_("Camping_Nooblet"), xorstr_("KNIFE_FIGHTER1337"), xorstr_("GRENADE_SPAMMER420"), xorstr_("One_Man_ArMYyy"),
    xorstr_("NooB_CRUSHER9000"), xorstr_("PrO_GaMeR69"), xorstr_("KeYbOaRd_WaRrIoR"), xorstr_("ClIcKeR_HeRo"), xorstr_("PoTaTo_AiM_"),
    xorstr_("WaLlHaCk_SuSpEcT"), xorstr_("LaG_LoRd"), xorstr_("DeSyNc_DeMoN"), xorstr_("MiCrOtRaNsAcTiOn_MaStEr"), xorstr_("GiT_GuD_ScRuB1337"),
    xorstr_("JoHn_WiCk"), xorstr_("RaMbO_ClOnE"), xorstr_("TeRmInAtOr_X"), xorstr_("IrOn_MaN_WaNnAbE"), xorstr_("CaPtAiN_AmErIcA"),
    xorstr_("DuMbLeDoRe_SpElLs"), xorstr_("GaNdAlF_ThE_GrEy"),
    xorstr_("M4A1_MaStEr"), xorstr_("RPG_Avenger"), xorstr_("RUSH_B_NO_STOP"), xorstr_("SMOKEGUN_Ninja"), xorstr_("CQB_Destroyer"),
    xorstr_("DEFUSE_Squad"), xorstr_("MEDIC_Here"), xorstr_("REvive_Me_PLS"), xorstr_("No_Mic_No_Skills"), xorstr_("Clutch_KING_"),
    xorstr_("Wallbang_WIZARD"), xorstr_("Spray_Control"), xorstr_("Headshot_Machine"), xorstr_("Ninja_Defuse"), xorstr_("Spawn_Peek_Master"),
    xorstr_("One_Tap_Wonder"), xorstr_("Tactical_Reload"), xorstr_("camper_KILLER"), xorstr_("Grenade_God"), xorstr_("Vehicle_Destroyer")
};