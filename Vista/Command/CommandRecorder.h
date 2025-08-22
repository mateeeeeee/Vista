#pragma once
#include "Command.h"
#include "RecorderManager.h"
#include "RTTI.h"
#include "Utilities/LinearAllocator.h"

namespace vista
{
	class RecorderManager; 
	class CommandRecorder
	{
	public:
		CommandRecorder(RecorderManager& recorderManager, ObjectID parentObjectId, CommandCategory category) : recorderManager(recorderManager), parentObjectId(parentObjectId), category(category) {}
		VISTA_NONCOPYABLE(CommandRecorder)
		VISTA_DEFAULT_MOVABLE(CommandRecorder)
		~CommandRecorder()
		{
			Clear();
		}

		template<typename CmdT, typename... Args> requires std::is_base_of_v<Command, CmdT>
		VISTA_MAYBE_UNUSED CmdT& AddCommand(Args&&... args)
		{
			CmdT* cmd = recorderManager.template AllocateCommand<CmdT>(std::forward<Args>(args)...);
			cmd->commandParentId = parentObjectId;
			cmd->commandIndex = recorderManager.GetNextCommandIndex();
			//VISTA_ASSERT_MSG(HasFlag(category, cmd->GetCategory()), "Category Mismatch in a Command Recorder!");
			commands.PushBack(cmd);
			return *cmd;
		}

		void Accept(CommandVisitor& v) const;

		template<typename F> requires std::is_invocable_v<F, Command const&>
		void ForAllCommands(F f) const
		{
			for (Command const& cmd : commands)
			{
				f(cmd);
			}
		}
		template<typename F> requires std::is_invocable_r_v<Bool, F, Command const&>
		void ForAllCommandsUntil(F f) const
		{
			for (Command const& cmd : commands)
			{
				if (f(cmd))
				{
					return;
				}
			}
		}

		template<typename CmdT> requires std::is_base_of_v<Command, CmdT>
		CmdT const* FindMostRecent() const
		{
			for (auto it = commands.rbegin(); it != commands.rend(); ++it) 
			{
				Command const& cmd = *it;
				if (CmdT const* command = dyn_cast<CmdT>(&cmd))
				{
					return command;
				}
			}
			return nullptr;
		}

		template<typename CmdT> requires std::is_base_of_v<Command, CmdT>
		Uint32 CommandCount() const
		{
			Uint32 count = 0;
			ForAllCommands([&count](Command const& cmd) 
				{
					if (isa<CmdT>(&cmd))
					{
						++count;
					}
				});
			return count;
		}

		Uint32 CommandCount() const
		{
			return (Uint32)commands.Size();
		}

		Bool IsTopLevel() const { return category != CommandCategory::List; }
		CommandCategory GetCategory() const { return category; }
		ObjectID GetParentObjectID() const { return parentObjectId; }

		Bool IsEmpty() const
		{
			return commands.Empty();
		}
		void Clear()
		{
			commands.Clear();
		}

		IList<Command> const& GetCommands() const { return commands; }

	private:
		RecorderManager& recorderManager;
		ObjectID parentObjectId;
		CommandCategory category;
		IList<Command> commands;
	};
}