# AI Resume Analyzer Pro (React + C++ REST API)
Upgraded portfolio project with:
- React premium frontend experience
- C++ REST API backend (`GET /health`, `POST /analyze`)
- Skill-match scoring + matched/missing terms + resume improvement tips
## Structure
- `frontend-react/` React + Vite UI
- `backend-cpp/` C++ HTTP API server
## Run Backend (C++)
```bash
cd backend-cpp
cmake -S . -B build
cmake --build build
./build/resume_analyzer_api
```
## Run Frontend (React)
```bash
cd frontend-react
npm install
npm run dev
```
API URL used by UI: `http://localhost:8081`
