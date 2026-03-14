from fastapi import FastAPI
from app.routes.analyze import router as analyze_router
app = FastAPI(title='AI Resume Analyzer Pro', version='1.0.0')
@app.get('/health')
def health():
    return {'ok': True, 'service': 'resume-analyzer'}
app.include_router(analyze_router)
