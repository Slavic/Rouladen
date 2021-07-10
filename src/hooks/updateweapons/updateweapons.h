#pragma once

typedef void(__fastcall* tUpdateWeapons)(int curtime);

extern tUpdateWeapons UpdateWeapons;

extern void __fastcall hUpdateWeapons(int curtime);
