#include "pch.h"
#include "Animation.h"

Animation::Animation(int rows, int columns, int frameCount, float frametime, bool loop)
	:
	m_Rows{ rows },
	m_Columns{ columns },
	m_FrameCount{ frameCount },
	m_CurrentFrame{},
	m_FrameTime{ frametime },
	m_AccumulatedTime{},
	m_Loop{ loop }
{
}
void Animation::Update(float elapsedSec)
{
	if (m_IsFinished || m_FrameCount <= 0 || m_FrameTime <= 0.f)
	{
		return;
	}

	m_AccumulatedTime += elapsedSec;

	if (m_AccumulatedTime >= m_FrameTime)
	{
		m_AccumulatedTime -= m_FrameTime;
		++m_CurrentFrame;

		if (m_CurrentFrame >= m_FrameCount)
		{
			if (m_Loop)
			{
				m_CurrentFrame = m_StartFrame;
			}
			else
			{
				m_CurrentFrame = m_FrameCount - 1;
				m_IsFinished = true;
			}
		}
	}
}

int Animation::GetCurrentFrame() const
{
	return m_CurrentFrame;
}
int Animation::GetCurrentRow() const
{
	return m_CurrentFrame / m_Columns;
}
int Animation::GetCurrentColumn() const
{
	return m_CurrentFrame % m_Columns;
}

int Animation::GetRows() const
{
	return m_Rows;
}

int Animation::GetColumns() const
{
	return m_Columns;
}

void Animation::SetStartFrame(int frame)
{
	// Unarmed enemy spritesheet is wrong

	if (m_FrameCount <= 0)
	{
		m_StartFrame = 0;
		m_CurrentFrame = 0;
		return;
	}

	if (frame < 0)
	{
		frame = 0;
	}

	if (frame >= m_FrameCount)
	{
		frame = m_FrameCount - 1;
	}

	m_StartFrame = frame;
	m_CurrentFrame = frame;
	m_AccumulatedTime = 0.f;
	m_IsFinished = false;
}

bool Animation::IsFinished() const
{
	return m_IsFinished;
}

void Animation::Reset()
{
	m_CurrentFrame = m_StartFrame;
	m_AccumulatedTime = 0.f;
	m_IsFinished = false;
}

void Animation::AdvanceFrame()
{
	if (m_CurrentFrame >= m_FrameCount - 1)
	{
		return;
	}
	++m_CurrentFrame;
}

bool Animation::IsOnLastFrame() const
{
	return m_CurrentFrame >= m_FrameCount - 1;
}

void Animation::SetCurrentFrame(int frame)
{
	if (m_FrameCount <= 0)
	{
		m_CurrentFrame = 0;
		return;
	}
	if (frame < 0)
	{
		frame = 0;
	}
	if (frame >= m_FrameCount)
	{
		frame = m_FrameCount - 1;
	}
	m_CurrentFrame = frame;
	m_AccumulatedTime = 0.f;
	m_IsFinished = false;
}

void Animation::RevereFrame()
{
	if (m_CurrentFrame <= m_StartFrame)
	{
		return;
	}
	--m_CurrentFrame;
}

bool Animation::IsOnStartFrame() const
{
	return m_CurrentFrame <= m_StartFrame;
}

