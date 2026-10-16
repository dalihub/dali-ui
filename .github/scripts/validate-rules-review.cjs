const fs = require('node:fs');

function readIssues(resultPath) {
  if (!fs.existsSync(resultPath)) {
    throw new Error('result.json을 생성하지 않았습니다.');
  }
  const stat = fs.lstatSync(resultPath);
  if (!stat.isFile() || stat.size > 1024 * 1024) {
    throw new Error('result.json은 1 MiB 이하의 일반 파일이어야 합니다.');
  }

  const issues = JSON.parse(fs.readFileSync(resultPath, 'utf8'));
  if (!Array.isArray(issues) || issues.length > 50) {
    throw new Error('result.json은 최대 50개 항목의 JSON array여야 합니다.');
  }

  const requiredFields = ['summary', 'rule', 'location', 'severity', 'problem', 'recommendation'];
  const allowedSeverities = new Set(['required', 'recommended', 'contextual', 'needs_confirmation']);
  for (const [index, issue] of issues.entries()) {
    if (!issue || typeof issue !== 'object' || Array.isArray(issue)) {
      throw new Error(`result.json item ${index + 1}이 object가 아닙니다.`);
    }
    for (const field of requiredFields) {
      if (typeof issue[field] !== 'string' || !issue[field].trim()) {
        throw new Error(`result.json item ${index + 1}의 ${field}는 비어 있지 않은 string이어야 합니다.`);
      }
    }
    if (!allowedSeverities.has(issue.severity)) {
      throw new Error(`result.json item ${index + 1}의 severity는 required, recommended, contextual, needs_confirmation 중 하나여야 합니다.`);
    }
  }
  return issues;
}

module.exports = { readIssues };

if (require.main === module) {
  try {
    readIssues(process.argv[2]);
  } catch (error) {
    console.error(error.message);
    process.exitCode = 1;
  }
}
