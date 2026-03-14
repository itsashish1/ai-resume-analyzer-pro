# AI Resume Analyzer Pro (React + C++)
This project is now built using:
- Frontend: React (Vite)
- Core engine: C++17 (resume-vs-JD scoring)
## Structure
- `frontend-react/` React UI
- `backend-cpp/` C++ analyzer core
## Run Frontend
```bash
cd frontend-react
npm install
npm run dev
```
## Build C++ Engine
```bash
cd backend-cpp
cmake -S . -B build
cmake --build build
./build/resume_analyzer
```
