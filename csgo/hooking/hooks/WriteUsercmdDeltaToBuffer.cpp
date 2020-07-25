#include "../../inc.h"

#include <algorithm>

bool __fastcall hook::WriteUserCmdDeltaToBuffer_hook(void* ECX, int nSlot, bf_write* buf, int from, int to, bool isNewCmd)
{
	static DWORD WriteUsercmdDeltaToBufferReturn = (DWORD)pattern::find(g_csgo.m_engine_dll, "84 C0 74 04 B0 01 EB 02 32 C0 8B FE 46 3B F3 7E C9 84 C0 0F 84"); // or 84 C0 74 04 B0 01 EB 02 32 C0 8B FE

	static auto call_SendMove = pattern::find(g_csgo.m_engine_dll, "84 C0 74 04 B0 01 EB 02 32 C0 8B FE 46 3B F3 7E C9 84 C0 0F 84 ? ? ? ?");

	static auto ofunct = g_hooks.m_clientmode.get_old_method< fn::WriteUsercmdDeltaToBuffer_hook_t >(hook::idx::WRITE_USERCMD_TO_DELTA);

	if (g_cl.m_tickbaseshift <= 0 || _ReturnAddress() != (void*)call_SendMove)
	{
		return ofunct(ECX, nSlot, buf, from, to, isNewCmd);
	}
	if (from != -1)
		return true;

	auto CLIENT_SendMove = []() {
		using CLIENT_SendMove_t = void(__fastcall*)(void);
		static CLIENT_SendMove_t CLIENT_SendMoveF = (CLIENT_SendMove_t)pattern::find(g_csgo.m_engine_dll, "55 8B EC A1 ? ? ? ? 81 EC ? ? ? ? B9 ? ? ? ? 53 8B 98");

		CLIENT_SendMoveF();
	};

	auto write_user_command = [](bf_write* buf, c_user_cmd* in, c_user_cmd* out) {
		using WriteUsercmd_t = void(__fastcall*)(bf_write*, c_user_cmd*, c_user_cmd*);
		static WriteUsercmd_t WriteUsercmdF = (WriteUsercmd_t)pattern::find(g_csgo.m_client_dll, "55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D");
		WriteUsercmdF(buf, in, out);

	};

	int* pointer_BackupCommands = (int*)((std::uintptr_t)(buf) - 0x30);
	int* pointer_NewCommands = (int*)((std::uintptr_t)(buf) - 0x2C);
	auto net_channel = *reinterpret_cast<i_net_channel * *>(reinterpret_cast<uintptr_t>(g_csgo.m_clientstate) + 0x9C);
	int32_t new_commands = *pointer_NewCommands;

	auto next_command_number = g_csgo.m_clientstate->m_last_outgoing_command + g_csgo.m_clientstate->m_choked_commands + 1;
	auto new_total_commands = std::min(g_cl.m_tickbaseshift, 16);

	g_cl.m_tickbaseshift -= new_total_commands;
	from = -1;

	*pointer_NewCommands = new_total_commands;
	*pointer_BackupCommands = 0;

	for (to = next_command_number - new_commands + 1; to <= next_command_number; to++)
	{
		if (!ofunct(ECX, nSlot, buf, from, to, true))
			return false;

		from = to;
	}

	c_user_cmd* last_real_command = g_cl.m_cmd->GetUserCmd(nSlot, from);

	c_user_cmd from_command;

	if (last_real_command)
		from_command = *last_real_command;

	c_user_cmd to_command = from_command;

	to_command.m_command_number++;
	to_command.m_tick_count += 200; //(TIME_TO_TICKS(0.5f) + next_command_number);

	for (int i = new_commands; i <= new_total_commands; i++)
	{
		write_user_command(buf, &to_command, &from_command);
		from_command = to_command;
		to_command.m_command_number++;
		to_command.m_tick_count++;
	}

	CLIENT_SendMove();

	net_channel->Transmit(true);

	return true;
}