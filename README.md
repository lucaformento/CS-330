# CS-330: Computational Graphics and Visualization
**Luca Formento | Southern New Hampshire University**

This repository contains my final project and design decisions document for CS-330.

---

## Included Files
- `7-1_FinalProjectMilestones.zip` — completed 3D scene (living room with lamp, TV, stand, and bookshelf)
- `CS330 Final Project Design Decisions - Luca Formento.docx` — 2-page design decisions reflection

---

## Reflection

### How do I approach designing software?

Working on this project helped me develop a clearer eye for breaking complex objects down into primitive shapes before writing a single line of code. The design process I followed was iterative: I started with Milestone One by proposing a scene and committing to a reference image, then built one object at a time across the milestones, adding textures in Module Five and lighting in Module Six, rather than trying to build everything at once. That staged approach kept each milestone manageable and meant each submission built cleanly on the last. Going forward I'd apply the same principle to any graphics or rendering work: commit to a scene concept early, decompose it into buildable units, and add complexity layer by layer rather than all at once.

### How do I approach developing programs?

The biggest new strategy I used was thinking in terms of transform order. Every object in the scene is placed using scale-then-rotate-then-translate, and understanding why that order matters (and what breaks when you get it wrong) changed how I reason about 3D space. Iteration was central to my development process: I ran the project after every meaningful change to catch issues early, which saved a lot of debugging time compared to making multiple changes at once and then having to figure out which one broke something. By the time I reached the final project, I had a reliable workflow: paste the code, retarget the toolset if needed, build, run, verify, then zip and submit. That repeatability made the later milestones much faster than the earlier ones.

### How can computer science help me in reaching my goals?

My long-term goal is working on AAA games, and this course gave me direct exposure to the rendering pipeline that sits under every modern game engine. The Phong lighting model, UV texture mapping, and camera transformation math I implemented in raw OpenGL map directly to how shaders and material systems work in Unreal Engine or Unity, just abstracted behind better tooling. Understanding what those engines are doing under the hood makes me a stronger developer when working at that level. On the educational side, this course connected concepts from linear algebra and data structures to a visual, real-time output in a way that made those abstract ideas concrete. That kind of connection is exactly what I need to keep building toward a career in game development.
