import re
STOPWORDS = {'the','is','a','an','for','and','or','to','of','in','on','with'}
def tokenize(text: str):
    words = re.findall(r'[a-zA-Z+#.]+', text.lower())
    return [w for w in words if w not in STOPWORDS and len(w) > 1]
def analyze_resume(resume_text: str, job_description: str):
    resume_tokens = set(tokenize(resume_text))
    jd_tokens = set(tokenize(job_description))
    if not jd_tokens:
        return {'score': 0, 'matched': [], 'missing': [], 'tips': ['Job description is empty.']}
    matched = sorted(resume_tokens.intersection(jd_tokens))
    missing = sorted(jd_tokens.difference(resume_tokens))
    score = round((len(matched) / len(jd_tokens)) * 100, 2)
    tips = []
    if score < 50:
      tips.append('Add quantifiable project outcomes and core skills from JD.')
    if 'project' not in resume_text.lower():
      tips.append('Include at least 2 detailed projects with stack and impact.')
    if 'intern' not in resume_text.lower():
      tips.append('Add internship or practical experience section.')
    return {
      'score': score,
      'matched': matched[:30],
      'missing': missing[:30],
      'tips': tips or ['Strong alignment. Keep resume concise and impact-focused.']
    }
