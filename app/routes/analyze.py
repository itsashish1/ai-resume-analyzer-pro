from fastapi import APIRouter
from pydantic import BaseModel
from app.services.scoring import analyze_resume
router = APIRouter(prefix='', tags=['analyze'])
class AnalyzeRequest(BaseModel):
    resume_text: str
    job_description: str
@router.post('/analyze')
def analyze(payload: AnalyzeRequest):
    return analyze_resume(payload.resume_text, payload.job_description)
