import React, { useMemo, useState } from "react";
import { createRoot } from "react-dom/client";
function tokenize(text) {
  return new Set((text.toLowerCase().match(/[a-z+#.]+/g) || []).filter((w) => w.length > 1));
}
function App() {
  const [resume, setResume] = useState("");
  const [jd, setJd] = useState("");
  const result = useMemo(() => {
    const r = tokenize(resume);
    const j = tokenize(jd);
    if (!j.size) return { score: 0, matched: [], missing: [] };
    const matched = [...j].filter((x) => r.has(x));
    const missing = [...j].filter((x) => !r.has(x));
    return { score: ((matched.length / j.size) * 100).toFixed(2), matched, missing };
  }, [resume, jd]);
  return (
    <div style={{ fontFamily: "Segoe UI, sans-serif", maxWidth: 980, margin: "24px auto", padding: 16 }}>
      <h1>AI Resume Analyzer (React UI)</h1>
      <p>Frontend in React + scoring logic available in C++ backend.</p>
      <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: 12 }}>
        <textarea rows={12} placeholder="Resume text" value={resume} onChange={(e) => setResume(e.target.value)} />
        <textarea rows={12} placeholder="Job description text" value={jd} onChange={(e) => setJd(e.target.value)} />
      </div>
      <h2>Score: {result.score}%</h2>
      <h3>Matched</h3>
      <p>{result.matched.join(", ") || "-"}</p>
      <h3>Missing</h3>
      <p>{result.missing.join(", ") || "-"}</p>
    </div>
  );
}
createRoot(document.getElementById("root")).render(<App />);
