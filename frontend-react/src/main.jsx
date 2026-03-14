import React, { useMemo, useState } from "react";
import { createRoot } from "react-dom/client";
const card = {
  background: "rgba(255,255,255,0.9)",
  borderRadius: 18,
  padding: 18,
  boxShadow: "0 10px 30px rgba(0,0,0,0.12)"
};
function App() {
  const [resume, setResume] = useState("");
  const [jd, setJd] = useState("");
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState("");
  const [apiResult, setApiResult] = useState(null);
  const fallback = useMemo(() => {
    const r = new Set((resume.toLowerCase().match(/[a-z+#.]+/g) || []).filter((w) => w.length > 1));
    const j = new Set((jd.toLowerCase().match(/[a-z+#.]+/g) || []).filter((w) => w.length > 1));
    if (!j.size) return { score: 0, matched: [], missing: [] };
    const matched = [...j].filter((x) => r.has(x));
    const missing = [...j].filter((x) => !r.has(x));
    return { score: ((matched.length / j.size) * 100).toFixed(2), matched, missing, tips: [] };
  }, [resume, jd]);
  const runAnalysis = async () => {
    setError("");
    setLoading(true);
    try {
      const res = await fetch("http://localhost:8081/analyze", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ resume_text: resume, job_description: jd })
      });
      if (!res.ok) throw new Error("API request failed");
      setApiResult(await res.json());
    } catch {
      setError("C++ API unavailable. Showing fallback score from frontend.");
      setApiResult(null);
    } finally {
      setLoading(false);
    }
  };
  const result = apiResult || fallback;
  return (
    <div style={{
      fontFamily: "'Trebuchet MS', 'Segoe UI', sans-serif",
      minHeight: "100vh",
      margin: 0,
      background: "radial-gradient(circle at 20% 20%, #f7e8ff 0%, #d9f1ff 40%, #fef6e4 100%)",
      padding: 20
    }}>
      <div style={{ maxWidth: 1100, margin: "0 auto" }}>
        <h1 style={{ fontSize: 42, marginBottom: 6 }}>Resume Analyzer Pro</h1>
        <p style={{ marginTop: 0 }}>React interface + C++ REST engine for recruiter-style skill matching.</p>
        <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: 14 }}>
          <div style={card}>
            <h3>Resume Text</h3>
            <textarea rows={14} value={resume} onChange={(e) => setResume(e.target.value)} style={{ width: "100%", padding: 10, borderRadius: 10, border: "1px solid #ddd" }} placeholder="Paste resume content" />
          </div>
          <div style={card}>
            <h3>Job Description</h3>
            <textarea rows={14} value={jd} onChange={(e) => setJd(e.target.value)} style={{ width: "100%", padding: 10, borderRadius: 10, border: "1px solid #ddd" }} placeholder="Paste JD content" />
          </div>
        </div>
        <div style={{ marginTop: 14, display: "flex", gap: 10, alignItems: "center" }}>
          <button onClick={runAnalysis} disabled={loading} style={{ padding: "12px 18px", borderRadius: 10, border: 0, background: "#0f766e", color: "white", fontWeight: 700, cursor: "pointer" }}>
            {loading ? "Analyzing..." : "Analyze with C++ API"}
          </button>
          {error && <span style={{ color: "#b91c1c" }}>{error}</span>}
        </div>
        <div style={{ ...card, marginTop: 14 }}>
          <h2 style={{ marginTop: 0 }}>Match Score: {Number(result.score).toFixed(2)}%</h2>
          <p><strong>Matched:</strong> {result.matched?.join(", ") || "-"}</p>
          <p><strong>Missing:</strong> {result.missing?.join(", ") || "-"}</p>
          {!!result.tips?.length && <p><strong>Tips:</strong> {result.tips.join(" | ")}</p>}
        </div>
      </div>
    </div>
  );
}
createRoot(document.getElementById("root")).render(<App />);
