# AI-SDLC Entry Point
**Say: `ai-sdlc start`** to begin.
See [ai-sdlc.md](ai-sdlc-core/ai-sdlc.md) for full workflow.

## Key Principles
1. **Evidence-based**: All verdicts need proof (tests, output, not reasoning)
2. **Gates matter**: Must stop at review gates, wait for approval
3. **State is truth**: Check `ai-sdlc-docs/state/state-log.md` to resume
4. **Workflow context persistence**: Once `ai-sdlc start`, all requests are processed 
   within the ai-sdlc workflow framework until explicitly stopped with `ai-sdlc stop`. 
   New feature requests, changes, or asks do NOT break the workflow—they are queued 
   or integrated into the current ai-sdlc phase.
