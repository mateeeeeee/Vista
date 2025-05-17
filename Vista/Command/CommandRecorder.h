#pragma once
#include "Command.h"
#include "RecorderManager.h"
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
			VISTA_ASSERT_MSG(HasFlag(category, cmd->GetCategory()), "Category Mismatch in a Command Recorder!");
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