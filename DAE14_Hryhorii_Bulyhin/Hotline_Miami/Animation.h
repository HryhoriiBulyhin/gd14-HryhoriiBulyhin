#pragma once
class Animation final
{
public:
	Animation() = default;
	explicit Animation(int rows, int columns, int frameCount, float frametime, bool loop);
	void Update(float elapsedSec);
	int GetCurrentFrame() const;
	int GetCurrentRow() const;
	int GetCurrentColumn() const;
	int GetRows() const;
	int GetColumns() const;

	void SetStartFrame(int frame);
	bool IsFinished()const;
	void Reset();

	void AdvanceFrame();
	bool IsOnLastFrame()const;

	void SetCurrentFrame(int frame);
	void RevereFrame();
	bool IsOnStartFrame()const;
private:
	int m_Rows{};
	int m_Columns{};
	int m_FrameCount{};
	int m_StartFrame{};

	int m_CurrentFrame{};
	float m_FrameTime{};
	float m_AccumulatedTime{};

	bool m_Loop{};
	bool m_IsFinished{};
};

